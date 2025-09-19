#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <memory>
#include <vector>
#include <random>
#include <optional>

#include "GameObject.hpp"
#include "Player.hpp"
#include "Enemy.hpp"
#include "Bullet.hpp"
#include "ParticleSystem.hpp"
#include "AudioSystem.hpp"
#include "ResourceManager.hpp"
#include "InputManager.hpp"
#include "PerformanceMonitor.hpp"
#include "ObjectPool.hpp"
#include "ExplosionAnimation.hpp"

class Game {
private:
    // Core systems
    sf::RenderWindow m_window;
    GameState m_currentState = GameState::MainMenu;
    ResourceManager m_resources;
    InputManager m_inputManager;
    AudioSystem m_audioSystem;
    PerformanceMonitor m_performanceMonitor;
    
    // Game objects with pools
    ObjectPool<Player> m_playerPool;
    ObjectPool<Enemy> m_enemyPool;
    ObjectPool<Bullet> m_bulletPool;
    Player* m_activePlayer = nullptr;
    std::vector<Enemy*> m_activeEnemies;
    std::vector<Bullet*> m_activeBullets;
    
    // Particle and explosion systems
    std::mt19937 m_rng{std::random_device{}()};
    ParticleSystem m_particleSystem{m_rng};
    ExplosionManager m_explosionManager;
    
    // Timing
    sf::Clock m_clock;
    float m_enemySpawnTimer = 0.0f;
    
    // Score tracking
    int m_score = 0;
    int m_enemiesKilled = 0;
    int m_previousScore = -1;
    int m_previousKills = -1;
    size_t m_previousEnemyCount = 0;
    
    // UI elements
    std::unique_ptr<sf::Text> m_scoreText;
    std::unique_ptr<sf::Text> m_killText;
    std::unique_ptr<sf::Text> m_enemyCountText;
    std::unique_ptr<sf::Text> m_fpsText;
    
    // Optimized grid rendering
    sf::VertexArray m_gridLines;
    
    // Background sprites
    std::optional<sf::Sprite> m_backgroundSprite;        // Game background
    std::optional<sf::Sprite> m_mainMenuBackgroundSprite; // Main menu background
    bool m_useBackgroundSprite = false;
    bool m_useMainMenuBackground = false;
    
    // Random distributions
    std::uniform_real_distribution<float> m_unitDist{0.0f, 1.0f};
    
public:
    Game();
    void run();
    
private:
    void initializeUI();
    void initializeGrid();
    void handleEvents();
    void handleKeyPress(sf::Keyboard::Key key);
    void startNewGame();
    void cleanup();
    void update(float deltaTime);
    void checkCollisions();
    void cleanupDeadObjects();
    void spawnEnemy();
    void shootBullet();
    void enemyShoot(Enemy* enemy);
    void render();
    void renderGame();
    void renderUI();
    void renderDebugInfo();
    void renderMainMenu();
    void renderPauseOverlay();
    void renderGameOver();
};