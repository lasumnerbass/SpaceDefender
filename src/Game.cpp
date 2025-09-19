#include "Game.hpp"
#include "Config.hpp"
#include "Utils.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

Game::Game() : m_window(sf::VideoMode({Config::WindowWidth, Config::WindowHeight}),
                       "Space Defender - Enhanced Edition") {
    m_window.setFramerateLimit(Config::TargetFPS);
    m_window.requestFocus();
    
    // Make window more prominent when running from IDE
    m_window.setPosition(sf::Vector2i(100, 100));
    
    // FIXED: Properly handle return values
    if (!m_resources.loadSystemFont()) {
        std::cerr << "Warning: Failed to load system font" << std::endl;
    }
    
    // Load textures
    m_resources.loadAllGameTextures();
    
    // Set up game background sprite if available
    if (m_resources.hasTexture("space_background")) {
        m_backgroundSprite = sf::Sprite(m_resources.getTexture("space_background"));
        m_useBackgroundSprite = true;
        std::cout << "Background sprite enabled" << std::endl;
    }
    
    // Set up main menu background sprite if available
    if (m_resources.hasTexture("main_menu_background")) {
        m_mainMenuBackgroundSprite = sf::Sprite(m_resources.getTexture("main_menu_background"));
        
        // Scale to fit window while maintaining aspect ratio
        sf::Vector2u textureSize = m_resources.getTexture("main_menu_background").getSize();
        float scaleX = static_cast<float>(Config::WindowWidth) / static_cast<float>(textureSize.x);
        float scaleY = static_cast<float>(Config::WindowHeight) / static_cast<float>(textureSize.y);
        float scale = std::max(scaleX, scaleY); // Use max to cover entire screen
        
        m_mainMenuBackgroundSprite->setScale(sf::Vector2f(scale, scale));
        
        // Center the background
        float scaledWidth = static_cast<float>(textureSize.x) * scale;
        float scaledHeight = static_cast<float>(textureSize.y) * scale;
        float offsetX = (static_cast<float>(Config::WindowWidth) - scaledWidth) / 2.0f;
        float offsetY = (static_cast<float>(Config::WindowHeight) - scaledHeight) / 2.0f;
        m_mainMenuBackgroundSprite->setPosition(sf::Vector2f(offsetX, offsetY));
        
        m_useMainMenuBackground = true;
        std::cout << "Main menu background sprite enabled" << std::endl;
    }
    
    // Initialize object pools
    m_playerPool.initialize(1);
    m_enemyPool.initialize(Config::EnemyPoolSize);
    m_bulletPool.initialize(Config::BulletPoolSize);
    
    // Reserve vectors
    m_activeEnemies.reserve(Config::MaxEnemies);
    m_activeBullets.reserve(Config::MaxBullets);
    
    // Initialize UI if font loaded
    if (m_resources.isFontLoaded()) {
        initializeUI();
    }
    
    // Initialize explosion manager
    m_explosionManager.initialize(m_resources);
    
    // Initialize grid
    initializeGrid();
    
    // Helpful message for users
    std::cout << "\n=== GAME READY ===" << std::endl;
    std::cout << "The game window should be open showing the main menu." << std::endl;
    std::cout << "Press '1' or ENTER in the game window to start playing!" << std::endl;
    std::cout << "Use WASD or arrow keys to move, SPACE or mouse to shoot." << std::endl;
}

void Game::run() {
    try {
        while (m_window.isOpen()) {
            float deltaTime = m_clock.restart().asSeconds();
            
            // Clamp delta time to prevent spiral of death
            deltaTime = std::min(deltaTime, Config::MaxDeltaTime);
            
            m_performanceMonitor.update(deltaTime);
            m_inputManager.update();
            
            handleEvents();
            update(deltaTime);
            render();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Game error: " << e.what() << std::endl;
    }
}

void Game::initializeUI() {
    const auto& font = m_resources.getFont();
    
    m_scoreText = std::make_unique<sf::Text>(font);
    m_scoreText->setCharacterSize(24);
    m_scoreText->setFillColor(sf::Color::White);
    m_scoreText->setPosition(sf::Vector2f(Config::WindowWidth - 200, 20));
    
    m_killText = std::make_unique<sf::Text>(font);
    m_killText->setCharacterSize(24);
    m_killText->setFillColor(sf::Color::Yellow);
    m_killText->setPosition(sf::Vector2f(Config::WindowWidth - 200, 50));
    
    m_enemyCountText = std::make_unique<sf::Text>(font);
    m_enemyCountText->setCharacterSize(24);
    m_enemyCountText->setFillColor(sf::Color::Red);
    m_enemyCountText->setPosition(sf::Vector2f(Config::WindowWidth - 200, 80));
    
    m_fpsText = std::make_unique<sf::Text>(font);
    m_fpsText->setCharacterSize(20);
    m_fpsText->setFillColor(sf::Color::Green);
    m_fpsText->setPosition(sf::Vector2f(10, Config::WindowHeight - 30));
}

void Game::initializeGrid() {
    m_gridLines.setPrimitiveType(sf::PrimitiveType::Lines);
    
    // Vertical lines
    for (unsigned int x = 0; x < Config::WindowWidth; x += 50) {
        m_gridLines.append(sf::Vertex(sf::Vector2f(static_cast<float>(x), 0), sf::Color(30, 30, 50)));
        m_gridLines.append(sf::Vertex(sf::Vector2f(static_cast<float>(x), Config::WindowHeight), sf::Color(30, 30, 50)));
    }
    
    // Horizontal lines
    for (unsigned int y = 0; y < Config::WindowHeight; y += 50) {
        m_gridLines.append(sf::Vertex(sf::Vector2f(0, static_cast<float>(y)), sf::Color(30, 30, 50)));
        m_gridLines.append(sf::Vertex(sf::Vector2f(Config::WindowWidth, static_cast<float>(y)), sf::Color(30, 30, 50)));
    }
}

void Game::handleEvents() {
    // FIXED: Properly handle optional event
    while (const std::optional<sf::Event> eventOpt = m_window.pollEvent()) {
        if (!eventOpt.has_value()) break;
        
        const sf::Event& event = eventOpt.value();
        
        if (event.is<sf::Event::Closed>()) {
            m_window.close();
        }
        
        if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
            m_inputManager.keyPressed(keyPressed->code);
            handleKeyPress(keyPressed->code);
        }
        
        if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()) {
            m_inputManager.keyReleased(keyReleased->code);
            
            if (keyReleased->code == sf::Keyboard::Key::Space && m_activePlayer) {
                m_activePlayer->setWantToShoot(false);
            }
        }
        
        if (event.is<sf::Event::FocusGained>() || event.is<sf::Event::FocusLost>()) {
            m_inputManager.clearAll();
        }
        
        if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
            m_inputManager.mousePressed(mousePressed->button);
            if (m_currentState == GameState::Playing && m_activePlayer && m_activePlayer->isAlive()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    shootBullet();
                }
            }
        }
        
        if (const auto* mouseButtonReleased = event.getIf<sf::Event::MouseButtonReleased>()) {
            m_inputManager.mouseReleased(mouseButtonReleased->button);
        }
    }
}

void Game::handleKeyPress(sf::Keyboard::Key key) {
    switch (m_currentState) {
        case GameState::MainMenu:
            if (key == sf::Keyboard::Key::Num1 || key == sf::Keyboard::Key::Enter) {
                startNewGame();
            } else if (key == sf::Keyboard::Key::Num2) {
                m_window.close();
            } else if (key == sf::Keyboard::Key::M) {
                m_audioSystem.setEnabled(!m_audioSystem.isEnabled());
            }
            break;
            
        case GameState::Playing:
            if (key == sf::Keyboard::Key::Escape) {
                m_currentState = GameState::Paused;
            } else if (key == sf::Keyboard::Key::Space && m_activePlayer) {
                m_activePlayer->setWantToShoot(true);
                shootBullet();
            } else if (key == sf::Keyboard::Key::M) {
                m_audioSystem.setEnabled(!m_audioSystem.isEnabled());
                std::cout << "Audio " << (m_audioSystem.isEnabled() ? "enabled" : "disabled") << std::endl;
            } else if (key == sf::Keyboard::Key::B) {
                // Toggle debug boundaries for testing (B key)
                static bool showBoundaries = false;
                showBoundaries = !showBoundaries;
                // This would require making ShowDebugBoundaries non-const, but for now just inform
                std::cout << "Debug boundaries toggle requested (requires code change to implement)" << std::endl;
            }
            break;
            
        case GameState::Paused:
            if (key == sf::Keyboard::Key::Escape || key == sf::Keyboard::Key::P) {
                m_currentState = GameState::Playing;
            } else if (key == sf::Keyboard::Key::M) {
                m_audioSystem.setEnabled(!m_audioSystem.isEnabled());
                std::cout << "Audio " << (m_audioSystem.isEnabled() ? "enabled" : "disabled") << std::endl;
            }
            break;
            
        case GameState::GameOver:
            if (key == sf::Keyboard::Key::Num1 || key == sf::Keyboard::Key::R) {
                startNewGame();
            } else if (key == sf::Keyboard::Key::Num2 || key == sf::Keyboard::Key::M) {
                m_currentState = GameState::MainMenu;
                cleanup();
            }
            break;
    }
}

void Game::startNewGame() {
    cleanup();
    
    m_currentState = GameState::Playing;
    m_score = 0;
    m_enemiesKilled = 0;
    m_enemySpawnTimer = 0.0f;
    m_previousScore = -1;
    m_previousKills = -1;
    m_previousEnemyCount = 0;
    m_performanceMonitor.reset();
    
    // Get player from pool
    m_activePlayer = m_playerPool.acquire();
    GAME_ASSERT(m_activePlayer != nullptr, "Failed to acquire player from pool");
    
    if (m_activePlayer) {
        m_activePlayer->initialize(sf::Vector2f(Config::WindowWidth / 2,
                                               Config::WindowHeight / 2));
        
        // Set player sprite if available
        if (m_resources.hasTexture("player_ship")) {
            m_activePlayer->setTexture(m_resources.getTexture("player_ship"), m_resources);
            std::cout << "Player sprite enabled" << std::endl;
        }
    }
    
    // Spawn initial enemies
    for (size_t i = 0; i < 2; ++i) {
        spawnEnemy();
    }
}

void Game::cleanup() {
    // Return objects to pools
    if (m_activePlayer) {
        m_playerPool.release(m_activePlayer);
        m_activePlayer = nullptr;
    }
    
    for (auto* enemy : m_activeEnemies) {
        m_enemyPool.release(enemy);
    }
    m_activeEnemies.clear();
    
    for (auto* bullet : m_activeBullets) {
        m_bulletPool.release(bullet);
    }
    m_activeBullets.clear();
    
    m_particleSystem.clear();
    m_inputManager.clearAll();
}

void Game::update(float deltaTime) {
    if (m_currentState != GameState::Playing) return;
    
    // Update player
    if (m_activePlayer && m_activePlayer->isAlive()) {
        m_activePlayer->update(deltaTime);
        m_activePlayer->updateMovement(m_inputManager, deltaTime);
        
        if (m_activePlayer->wantsToShoot() ||
            m_inputManager.isMousePressed(sf::Mouse::Button::Left)) {
            shootBullet();
        }
    } else if (m_activePlayer && !m_activePlayer->isAlive()) {
        m_explosionManager.createExplosion(m_activePlayer->getPosition(), ExplosionAnimation::ExplosionType::Large);
        m_particleSystem.emit(m_activePlayer->getPosition(), sf::Color::Cyan, 30);
        m_audioSystem.playExplosion();
        m_currentState = GameState::GameOver;
    }
    
    // Update enemies
    for (auto* enemy : m_activeEnemies) {
        enemy->update(deltaTime);
        enemy->updateAI(m_rng, deltaTime);
        
        // Make enemies face the player for better visual targeting
        if (m_activePlayer && m_activePlayer->isAlive()) {
            enemy->faceTarget(m_activePlayer->getPosition());
            
            if (enemy->canShoot()) {
                enemyShoot(enemy);
            }
        }
    }
    
    // Update bullets
    for (auto* bullet : m_activeBullets) {
        bullet->update(deltaTime);
    }
    
    // Update particles and explosions
    m_particleSystem.update(deltaTime);
    m_explosionManager.update(deltaTime);
    
    // Handle collisions
    checkCollisions();
    
    // Clean up dead objects and finished explosions
    cleanupDeadObjects();
    m_explosionManager.cleanup();
    
    // Spawn enemies
    m_enemySpawnTimer += deltaTime;
    if (m_enemySpawnTimer > Config::EnemySpawnCooldown &&
        m_activeEnemies.size() < Config::MaxEnemies) {
        spawnEnemy();
        m_enemySpawnTimer = 0.0f;
    }
}

void Game::checkCollisions() {
    for (auto* bullet : m_activeBullets) {
        if (!bullet->isAlive()) continue;
        
        if (bullet->isPlayerBullet()) {
            // Check against enemies
            for (auto* enemy : m_activeEnemies) {
                if (!enemy->isAlive()) continue;
                
                if (Utils::rectIntersects(bullet->getBounds(), enemy->getBounds())) {
                    enemy->takeDamage(bullet->getDamage());
                    bullet->setAlive(false);
                    
                    // Small explosion at bullet impact point
                    m_explosionManager.createExplosion(bullet->getPosition(), ExplosionAnimation::ExplosionType::Small);
                    
                    m_audioSystem.playHit();
                    
                    if (!enemy->isAlive()) {
                        m_score += enemy->getScoreValue();
                        m_enemiesKilled++;
                        
                        // Create explosion based on enemy type
                        ExplosionAnimation::ExplosionType explosionType;
                        switch (enemy->getType()) {
                            case EnemyType::BasicEnemy:
                                explosionType = ExplosionAnimation::ExplosionType::Medium;
                                break;
                            case EnemyType::FastEnemy:
                                explosionType = ExplosionAnimation::ExplosionType::Small;
                                break;
                            case EnemyType::HeavyEnemy:
                                explosionType = ExplosionAnimation::ExplosionType::Large;
                                break;
                        }
                        
                        m_explosionManager.createExplosion(enemy->getPosition(), explosionType);
                        m_particleSystem.emit(enemy->getPosition(), enemy->getColor(), 20);
                        m_audioSystem.playExplosion();
                    }
                    break;
                }
            }
        } else {
            // Check against player
            if (m_activePlayer && m_activePlayer->isAlive()) {
                if (Utils::rectIntersects(bullet->getBounds(), m_activePlayer->getBounds())) {
                    m_activePlayer->takeDamage(bullet->getDamage());
                    bullet->setAlive(false);
                    
                    // Small explosion at bullet impact point
                    m_explosionManager.createExplosion(bullet->getPosition(), ExplosionAnimation::ExplosionType::Small);
                    
                    m_particleSystem.emit(bullet->getPosition(), sf::Color::Red, 5);
                    m_audioSystem.playHit();
                }
            }
        }
    }
}

void Game::cleanupDeadObjects() {
    // Return dead enemies to pool
    auto enemyEnd = std::partition(m_activeEnemies.begin(), m_activeEnemies.end(),
        [](Enemy* e) { return e->isAlive(); });
    for (auto it = enemyEnd; it != m_activeEnemies.end(); ++it) {
        m_enemyPool.release(*it);
    }
    m_activeEnemies.erase(enemyEnd, m_activeEnemies.end());
    
    // Return dead bullets to pool
    auto bulletEnd = std::partition(m_activeBullets.begin(), m_activeBullets.end(),
        [](Bullet* b) { return b->isAlive(); });
    for (auto it = bulletEnd; it != m_activeBullets.end(); ++it) {
        m_bulletPool.release(*it);
    }
    m_activeBullets.erase(bulletEnd, m_activeBullets.end());
}

void Game::spawnEnemy() {
    if (m_activeEnemies.size() >= Config::MaxEnemies) return;
    
    Enemy* enemy = m_enemyPool.acquire();
    if (!enemy) {
        GAME_ASSERT(false, "Enemy pool exhausted!");
        return;
    }
    
    // Find spawn position away from player
    float x = m_unitDist(m_rng) * Config::WindowWidth;
    float y = m_unitDist(m_rng) * Config::WindowHeight;
    
    if (m_activePlayer) {
        const sf::Vector2f& playerPos = m_activePlayer->getPosition();
        constexpr float MinDistance = 150.0f;
        constexpr float MinDistanceSquared = MinDistance * MinDistance;
        
        int attempts = 0;
        while (Utils::distanceSquared(sf::Vector2f(x, y), playerPos) < MinDistanceSquared &&
               attempts++ < 10) {
            x = m_unitDist(m_rng) * Config::WindowWidth;
            y = m_unitDist(m_rng) * Config::WindowHeight;
        }
    }
    
    // Choose enemy type
    EnemyType type;
    const float rand = m_unitDist(m_rng);
    if (rand < 0.5f) {
        type = EnemyType::BasicEnemy;
    } else if (rand < 0.8f) {
        type = EnemyType::FastEnemy;
    } else {
        type = EnemyType::HeavyEnemy;
    }
    
    enemy->initialize(sf::Vector2f(x, y), type, m_rng);
    
    // Set appropriate sprite based on enemy type
    std::string spriteKey;
    switch (type) {
        case EnemyType::BasicEnemy:
            spriteKey = "enemy_basic";
            break;
        case EnemyType::FastEnemy:
            spriteKey = "enemy_fast";
            break;
        case EnemyType::HeavyEnemy:
            spriteKey = "enemy_heavy";
            break;
    }
    
    if (m_resources.hasTexture(spriteKey)) {
        enemy->setTexture(m_resources.getTexture(spriteKey), m_resources);
    }
    
    m_activeEnemies.push_back(enemy);
}

void Game::shootBullet() {
    if (!m_activePlayer || !m_activePlayer->canShoot()) return;
    if (m_activeBullets.size() >= Config::MaxBullets) return;
    
    Bullet* bullet = m_bulletPool.acquire();
    if (bullet) {
        bullet->initialize(m_activePlayer->getShootPosition(),
                         m_activePlayer->getShootDirection(), true);
        
        // Apply player bullet sprite if available
        if (m_resources.hasTexture("player_bullet")) {
            bullet->setTexture(m_resources.getTexture("player_bullet"), m_resources);
        }
        
        m_activeBullets.push_back(bullet);
        m_activePlayer->resetShootCooldown();
        m_audioSystem.playShoot();
    }
}

void Game::enemyShoot(Enemy* enemy) {
    if (!enemy || !m_activePlayer || m_activeBullets.size() >= Config::MaxBullets) return;
    
    Bullet* bullet = m_bulletPool.acquire();
    if (bullet) {
        bullet->initialize(enemy->getCenteredShootPosition(m_activePlayer->getPosition()),
                         enemy->getShootDirection(m_activePlayer->getPosition()), false);
        
        // Apply enemy bullet sprite based on enemy type
        std::string bulletTextureKey;
        switch (enemy->getType()) {
            case EnemyType::BasicEnemy:
            case EnemyType::FastEnemy:
                bulletTextureKey = "enemy_bullet";
                break;
            case EnemyType::HeavyEnemy:
                bulletTextureKey = "heavy_bullet";
                break;
        }
        
        if (m_resources.hasTexture(bulletTextureKey)) {
            bullet->setTexture(m_resources.getTexture(bulletTextureKey), m_resources);
        }
        
        m_activeBullets.push_back(bullet);
        enemy->resetShootCooldown();
        m_audioSystem.playShoot();
    }
}

void Game::render() {
    m_window.clear(sf::Color(10, 10, 30));
    
    switch (m_currentState) {
        case GameState::MainMenu:
            renderMainMenu();
            break;
            
        case GameState::Playing:
        case GameState::Paused:
            renderGame();
            if (m_currentState == GameState::Paused) {
                renderPauseOverlay();
            }
            break;
            
        case GameState::GameOver:
            renderGame();
            renderGameOver();
            break;
    }
    
    m_window.display();
}

void Game::renderGame() {
    // Draw background
    if (m_useBackgroundSprite && m_backgroundSprite) {
        m_window.draw(*m_backgroundSprite);
    } else {
        // Fallback to grid
        m_window.draw(m_gridLines);
    }
    
    // Draw explosions first (behind everything except background)
    m_explosionManager.draw(m_window);
    
    // Draw particles
    m_particleSystem.draw(m_window);
    
    // Draw enemies
    for (const auto* enemy : m_activeEnemies) {
        enemy->draw(m_window);
    }
    
    // Draw player
    if (m_activePlayer && m_activePlayer->isAlive()) {
        m_activePlayer->draw(m_window);
        m_activePlayer->drawHealthBar(m_window);
    }
    
    // Draw bullets
    for (const auto* bullet : m_activeBullets) {
        bullet->draw(m_window);
    }
    
    // Draw UI
    renderUI();
    
    // Draw performance stats
    if (Config::ShowDebugInfo) {
        renderDebugInfo();
    }
}

void Game::renderUI() {
    if (!m_resources.isFontLoaded()) return;
    
    // Only update text when values change
    if (m_scoreText && m_score != m_previousScore) {
        m_scoreText->setString("SCORE: " + std::to_string(m_score));
        m_previousScore = m_score;
    }
    
    if (m_killText && m_enemiesKilled != m_previousKills) {
        m_killText->setString("KILLS: " + std::to_string(m_enemiesKilled));
        m_previousKills = m_enemiesKilled;
    }
    
    if (m_enemyCountText && m_activeEnemies.size() != m_previousEnemyCount) {
        m_enemyCountText->setString("ENEMIES: " + std::to_string(m_activeEnemies.size()) + "/" +
                                   std::to_string(Config::MaxEnemies));
        m_previousEnemyCount = m_activeEnemies.size();
    }
    
    if (m_scoreText) m_window.draw(*m_scoreText);
    if (m_killText) m_window.draw(*m_killText);
    if (m_enemyCountText) m_window.draw(*m_enemyCountText);
    
    // Update FPS display
    if (m_fpsText) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1)
            << "FPS: " << m_performanceMonitor.getAverageFPS();
        m_fpsText->setString(oss.str());
        m_window.draw(*m_fpsText);
    }
}

void Game::renderDebugInfo() {
    if (!m_resources.isFontLoaded()) return;
    
    sf::Text debugText(m_resources.getFont());
    debugText.setCharacterSize(16);
    debugText.setFillColor(sf::Color(128, 255, 128));
    debugText.setPosition(sf::Vector2f(10, 40));
    
    std::ostringstream oss;
    oss << "== DEBUG INFO ==\n"
        << "Bullets: " << m_activeBullets.size() << "/" << m_bulletPool.getStats().totalAllocated << "\n"
        << "Enemies: " << m_activeEnemies.size() << "/" << m_enemyPool.getStats().totalAllocated << "\n"
        << "Particles: " << m_particleSystem.getActiveCount() << "/" << Config::MaxParticles << "\n"
        << "Pool Usage:\n"
        << "  Bullet: " << std::fixed << std::setprecision(1) << (m_bulletPool.getUsageRatio() * 100) << "%\n"
        << "  Enemy: " << (m_enemyPool.getUsageRatio() * 100) << "%\n"
        << "Audio: " << (m_audioSystem.isEnabled() ? "ON" : "OFF");
    
    debugText.setString(oss.str());
    m_window.draw(debugText);
}

void Game::renderMainMenu() {
    // Draw main menu background if available
    if (m_useMainMenuBackground && m_mainMenuBackgroundSprite) {
        m_window.draw(*m_mainMenuBackgroundSprite);
    }
    
    if (!m_resources.isFontLoaded()) return;
    
    // Add semi-transparent overlay for better text readability over space background
    if (m_useMainMenuBackground) {
        sf::RectangleShape overlay(sf::Vector2f(Config::WindowWidth, Config::WindowHeight));
        overlay.setFillColor(sf::Color(0, 0, 0, 100)); // Semi-transparent black
        m_window.draw(overlay);
    }
    
    sf::Text title(m_resources.getFont());
    title.setString("SPACE DEFENDER");
    title.setCharacterSize(72);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Style::Bold); // Make title bold for better visibility
    title.setPosition(sf::Vector2f(Config::WindowWidth / 2 - 300, 100));
    
    // Add subtle outline/shadow effect for title
    sf::Text titleShadow = title;
    titleShadow.setFillColor(sf::Color::Black);
    titleShadow.setPosition(sf::Vector2f(Config::WindowWidth / 2 - 298, 102));
    m_window.draw(titleShadow);
    m_window.draw(title);
    
    sf::Text menu(m_resources.getFont());
    menu.setString(
        "\n\n\n\n\n1 or ENTER - START GAME\n"
        "2 - EXIT\n"
        "M - TOGGLE SOUND\n\n"
        "CONTROLS:\n"
        "WASD or Arrow Keys - Movement\n"
        "SPACE or Left Mouse - Shoot\n"
        "ESC - Pause"
    );
    menu.setCharacterSize(24);
    menu.setFillColor(sf::Color::White);
    menu.setPosition(sf::Vector2f(Config::WindowWidth / 2 - 200, 250));
    m_window.draw(menu);
}

void Game::renderPauseOverlay() {
    sf::RectangleShape overlay(sf::Vector2f(Config::WindowWidth, Config::WindowHeight));
    overlay.setFillColor(sf::Color(0, 0, 0, 128));
    m_window.draw(overlay);
    
    if (m_resources.isFontLoaded()) {
        sf::Text text(m_resources.getFont());
        text.setString("PAUSED\n\nPress ESC or P to Continue");
        text.setCharacterSize(48);
        text.setFillColor(sf::Color::White);
        text.setPosition(sf::Vector2f(Config::WindowWidth / 2 - 200, Config::WindowHeight / 2 - 50));
        m_window.draw(text);
    }
}

void Game::renderGameOver() {
    sf::RectangleShape overlay(sf::Vector2f(Config::WindowWidth, Config::WindowHeight));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    m_window.draw(overlay);
    
    if (m_resources.isFontLoaded()) {
        sf::Text text(m_resources.getFont());
        text.setString("GAME OVER\n\nFINAL SCORE: " + std::to_string(m_score) +
                      "\n\n1 or R - PLAY AGAIN\n2 or M - MAIN MENU");
        text.setCharacterSize(48);
        text.setFillColor(sf::Color::Red);
        text.setPosition(sf::Vector2f(Config::WindowWidth / 2 - 250, Config::WindowHeight / 2 - 150));
        m_window.draw(text);
    }
}
