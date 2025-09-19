#include "ExplosionAnimation.hpp"
#include "ResourceManager.hpp"
#include "Config.hpp"
#include <iostream>
#include <algorithm>

// ExplosionAnimation Implementation
ExplosionAnimation::ExplosionAnimation()
    : m_position(0.0f, 0.0f)
    , m_type(ExplosionType::Medium)
    , m_sprite(std::nullopt)  // Initialize optional sprite as nullopt
    , m_active(false)
    , m_frameTime(0.1f)  // 10 FPS by default
    , m_currentFrameTime(0.0f)
    , m_currentFrame(0)
    , m_totalFrames(16)  // Assuming 4x4 sprite sheet by default
    , m_framesPerRow(4)
    , m_frameWidth(64)   // Will be calculated from texture
    , m_frameHeight(64)  // Will be calculated from texture
    , m_scale(1.0f) {
}

void ExplosionAnimation::initialize(const sf::Vector2f& position, ExplosionType type, const sf::Texture& texture) {
    m_position = position;
    m_type = type;
    m_active = true;
    m_currentFrame = 0;
    m_currentFrameTime = 0.0f;
    
    // Set up sprite with texture
    m_sprite = sf::Sprite(texture);
    
    // Configure explosion based on sprite sheet
    // Assuming the explosion sprite sheet you provided is 4x4 = 16 frames
    m_framesPerRow = 4;
    m_totalFrames = 16;
    m_frameWidth = static_cast<int>(texture.getSize().x) / m_framesPerRow;
    m_frameHeight = static_cast<int>(texture.getSize().y) / (m_totalFrames / m_framesPerRow);
    
    // Configure type-specific parameters
    configureForType(type);
    
    // Set sprite properties
    m_sprite->setScale(sf::Vector2f(m_scale, m_scale));
    m_sprite->setOrigin(sf::Vector2f(static_cast<float>(m_frameWidth) / 2.0f, static_cast<float>(m_frameHeight) / 2.0f));
    m_sprite->setPosition(m_position);
    
    // Set initial frame
    updateSpriteFrame();
    
    std::cout << "Explosion initialized at (" << position.x << ", " << position.y << ") with " 
              << m_totalFrames << " frames" << std::endl;
}

void ExplosionAnimation::configureForType(ExplosionType type) {
    switch (type) {
        case ExplosionType::Small:
            m_scale = 0.6f;
            m_frameTime = 0.06f;  // Fast, snappy animation for bullet hits
            break;
        case ExplosionType::Medium:
            m_scale = 1.0f;
            m_frameTime = 0.08f;  // Medium speed for basic enemies
            break;
        case ExplosionType::Large:
            m_scale = 1.5f;
            m_frameTime = 0.1f;   // Slower, more dramatic for player/heavy enemies
            break;
    }
}

void ExplosionAnimation::update(float deltaTime) {
    if (!m_active) return;
    
    m_currentFrameTime += deltaTime;
    
    if (m_currentFrameTime >= m_frameTime) {
        m_currentFrameTime -= m_frameTime;
        m_currentFrame++;
        
        if (m_currentFrame >= m_totalFrames) {
            // Animation finished
            m_active = false;
            return;
        }
        
        updateSpriteFrame();
    }
}

void ExplosionAnimation::updateSpriteFrame() {
    if (!m_sprite) return;
    
    int row = m_currentFrame / m_framesPerRow;
    int col = m_currentFrame % m_framesPerRow;
    
    sf::IntRect frameRect(
        sf::Vector2i(col * m_frameWidth, row * m_frameHeight),
        sf::Vector2i(m_frameWidth, m_frameHeight)
    );
    
    m_sprite->setTextureRect(frameRect);
}

void ExplosionAnimation::draw(sf::RenderWindow& window) const {
    if (m_active && m_sprite) {
        window.draw(*m_sprite);
    }
}

void ExplosionAnimation::reset() {
    m_active = false;
    m_currentFrame = 0;
    m_currentFrameTime = 0.0f;
}

// ExplosionManager Implementation
ExplosionManager::ExplosionManager() 
    : m_explosionTexture(nullptr) {
}

void ExplosionManager::initialize(const ResourceManager& resourceManager) {
    try {
        if (resourceManager.hasTexture("explosion")) {
            m_explosionTexture = &resourceManager.getTexture("explosion");
            std::cout << "Explosion texture loaded successfully" << std::endl;
        } else {
            std::cout << "Warning: No explosion texture found. Explosions will be disabled." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to load explosion texture: " << e.what() << std::endl;
        m_explosionTexture = nullptr;
    }
}

void ExplosionManager::createExplosion(const sf::Vector2f& position, ExplosionAnimation::ExplosionType type) {
    if (!m_explosionTexture) {
        std::cout << "No explosion texture available - skipping explosion effect" << std::endl;
        return;
    }
    
    ExplosionAnimation* explosion = getAvailableExplosion();
    if (explosion) {
        explosion->initialize(position, type, *m_explosionTexture);
    }
}

void ExplosionManager::update(float deltaTime) {
    for (auto& explosion : m_explosions) {
        if (explosion->isActive()) {
            explosion->update(deltaTime);
        }
    }
}

void ExplosionManager::draw(sf::RenderWindow& window) const {
    for (const auto& explosion : m_explosions) {
        if (explosion->isActive()) {
            explosion->draw(window);
        }
    }
}

void ExplosionManager::cleanup() {
    // Remove finished explosions to prevent unlimited growth
    // Keep a reasonable number of explosion objects for reuse
    const size_t maxPoolSize = 20;
    
    if (m_explosions.size() > maxPoolSize) {
        auto newEnd = std::remove_if(m_explosions.begin(), m_explosions.end(),
            [](const std::unique_ptr<ExplosionAnimation>& explosion) {
                return !explosion->isActive();
            });
        
        // Keep only the first maxPoolSize explosions
        if (static_cast<size_t>(std::distance(m_explosions.begin(), newEnd)) > maxPoolSize) {
            m_explosions.erase(m_explosions.begin() + maxPoolSize, m_explosions.end());
        }
    }
}

ExplosionAnimation* ExplosionManager::getAvailableExplosion() {
    // Look for an inactive explosion to reuse
    for (auto& explosion : m_explosions) {
        if (!explosion->isActive()) {
            return explosion.get();
        }
    }
    
    // Create new explosion if none available
    m_explosions.push_back(std::make_unique<ExplosionAnimation>());
    return m_explosions.back().get();
}