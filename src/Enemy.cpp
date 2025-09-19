#include "Enemy.hpp"
#include "ResourceManager.hpp"
#include "Config.hpp"
#include "Utils.hpp"
#include <random>
#include <algorithm>

Enemy::Enemy() {
    // Default constructor - sprite will be created when texture is set
}

void Enemy::setTexture(const sf::Texture& texture, const ResourceManager& resourceManager) {
    m_sprite = sf::Sprite(texture);
    
    // Use exact sizing to force all sprites to identical pixel dimensions (same as player)
    resourceManager.scaleTextureToExactSize(*m_sprite, Config::PlayerSpriteSize, Config::PlayerSpriteSize);
    
    m_useSprites = true;
}

void Enemy::enableSpriteMode(bool enable) {
    m_useSprites = enable;
}

void Enemy::initialize(const sf::Vector2f& position, EnemyType type, std::mt19937& rng) noexcept {
    m_position = position;
    m_type = type;
    m_alive = true;
    m_moveTimer = 0.0f;
    
    switch (m_type) {
        case EnemyType::BasicEnemy:
            m_maxHealth = 50.0f;
            m_speed = 100.0f;
            m_shootCooldown = 2.0f;
            m_shape.setRadius(15.0f);
            m_baseColor = sf::Color::Magenta;
            break;
            
        case EnemyType::FastEnemy:
            m_maxHealth = 30.0f;
            m_speed = 200.0f;
            m_shootCooldown = 1.5f;
            m_shape.setRadius(10.0f);
            m_baseColor = sf::Color::Green;
            break;
            
        case EnemyType::HeavyEnemy:
            m_maxHealth = 100.0f;
            m_speed = 50.0f;
            m_shootCooldown = 3.0f;
            m_shape.setRadius(20.0f);
            m_baseColor = sf::Color(255, 128, 0);
            break;
    }
    
    m_currentHealth = m_maxHealth;
    m_currentShootCooldown = m_shootCooldown;
    
    m_shape.setFillColor(m_baseColor);
    m_shape.setOutlineThickness(2.0f);
    m_shape.setOutlineColor(sf::Color::White);
    m_shape.setOrigin(sf::Vector2f(m_shape.getRadius(), m_shape.getRadius()));
    m_shape.setPosition(m_position);
    
    // Sprite scaling is now handled in setTexture method
    if (m_useSprites && m_sprite) {
        m_sprite->setPosition(m_position);
    }
    
    std::uniform_real_distribution<float> angleDist(0, Config::TwoPi);
    const float angle = angleDist(rng);
    m_targetDirection = sf::Vector2f(std::cos(angle), std::sin(angle));
}

void Enemy::update(float deltaTime) {
    if (!m_alive) return;
    
    m_currentShootCooldown = std::max(0.0f, m_currentShootCooldown - deltaTime);
    m_moveTimer += deltaTime;
    
    m_velocity = m_targetDirection * m_speed;
    m_position += m_velocity * deltaTime;
    
    // Enemies should face the player (shooting direction), not movement direction
    // This will be updated when we have a target to aim at
    
    const float radius = m_shape.getRadius();
    if (m_position.x < radius || m_position.x > Config::WindowWidth - radius) {
        m_targetDirection.x = -m_targetDirection.x;
        m_position.x = Utils::clamp(m_position.x, radius, Config::WindowWidth - radius);
    }
    if (m_position.y < radius || m_position.y > Config::WindowHeight - radius) {
        m_targetDirection.y = -m_targetDirection.y;
        m_position.y = Utils::clamp(m_position.y, radius, Config::WindowHeight - radius);
    }
    
    m_shape.setPosition(m_position);
    if (m_sprite) {
        m_sprite->setPosition(m_position);
        m_sprite->setRotation(sf::degrees(m_rotation + Config::SpriteOrientationOffset));
    }
    
    if (m_currentHealth < m_maxHealth) {
        const float healthPercent = m_currentHealth / m_maxHealth;
        const std::uint8_t alpha = static_cast<std::uint8_t>(128 + 127 * healthPercent);
        sf::Color flashColor = m_baseColor;
        flashColor.a = alpha;
        m_shape.setFillColor(flashColor);
    }
}

void Enemy::updateAI(std::mt19937& rng, float deltaTime) noexcept {
    if (m_moveTimer > 2.0f) {
        std::uniform_real_distribution<float> timeDist(0, 2);
        std::uniform_real_distribution<float> angleDist(0, Config::TwoPi);
        
        if (m_moveTimer > 2.0f + timeDist(rng)) {
            m_moveTimer = 0.0f;
            const float angle = angleDist(rng);
            m_targetDirection = sf::Vector2f(std::cos(angle), std::sin(angle));
        }
    }
}

void Enemy::faceTarget(const sf::Vector2f& targetPosition) noexcept {
    // Calculate direction to target
    sf::Vector2f direction = targetPosition - m_position;
    if (direction.x != 0 || direction.y != 0) {
        m_rotation = Utils::radToDeg(std::atan2(direction.y, direction.x));
        
        // Update sprite rotation if using sprites (with orientation offset)
        if (m_sprite) {
            m_sprite->setRotation(sf::degrees(m_rotation + Config::SpriteOrientationOffset));
        }
    }
}

void Enemy::draw(sf::RenderWindow& window) const {
    if (m_alive) {
        if (m_useSprites && m_sprite) {
            // Apply damage tinting to sprite
            if (m_currentHealth < m_maxHealth) {
                const float healthPercent = m_currentHealth / m_maxHealth;
                const std::uint8_t alpha = static_cast<std::uint8_t>(128 + 127 * healthPercent);
                sf::Color tintColor = sf::Color::White;
                tintColor.a = alpha;
                const_cast<Enemy*>(this)->m_sprite->setColor(tintColor);
            } else {
                const_cast<Enemy*>(this)->m_sprite->setColor(sf::Color::White);
            }
            window.draw(*m_sprite);
            
            // Draw debug boundaries if enabled
            if (Config::ShowDebugBoundaries) {
                drawDebugBounds(window);
            }
        } else {
            window.draw(m_shape); // Fallback to geometric rendering
            
            // Draw debug boundaries for geometric shape too
            if (Config::ShowDebugBoundaries) {
                drawDebugBounds(window);
            }
        }
    }
}

void Enemy::drawDebugBounds(sf::RenderWindow& window) const noexcept {
    // Different colors for different enemy types
    sf::Color outlineColor;
    switch (m_type) {
        case EnemyType::BasicEnemy:
            outlineColor = sf::Color(255, 0, 255, 192); // Bright magenta
            break;
        case EnemyType::FastEnemy:
            outlineColor = sf::Color(0, 255, 255, 192); // Bright cyan
            break;
        case EnemyType::HeavyEnemy:
            outlineColor = sf::Color(255, 128, 0, 192); // Bright orange
            break;
    }
    
    // Draw only the tight bounds (actual collision bounds) for clarity
    sf::FloatRect tightBounds = getTightBounds();
    sf::RectangleShape tightDebugRect(tightBounds.size);
    tightDebugRect.setPosition(tightBounds.position);
    tightDebugRect.setFillColor(sf::Color::Transparent);
    tightDebugRect.setOutlineThickness(2.0f);
    tightDebugRect.setOutlineColor(outlineColor);
    window.draw(tightDebugRect);
}

sf::FloatRect Enemy::getBounds() const {
    // Use tight bounds for actual collision detection
    return getTightBounds();
}

sf::FloatRect Enemy::getTightBounds() const noexcept {
    sf::FloatRect originalBounds;
    if (m_useSprites && m_sprite) {
        originalBounds = m_sprite->getGlobalBounds();
    } else {
        originalBounds = m_shape.getGlobalBounds();
    }
    
    // Create tighter bounds by reducing size and centering
    sf::Vector2f originalSize = originalBounds.size;
    sf::Vector2f tightSize = originalSize * Config::SpriteBoundsTightnessRatio;
    sf::Vector2f sizeReduction = originalSize - tightSize;
    sf::Vector2f positionOffset = sizeReduction * 0.5f; // Center the tight bounds
    
    return sf::FloatRect(originalBounds.position + positionOffset, tightSize);
}

bool Enemy::canShoot() const noexcept {
    return m_currentShootCooldown <= 0.0f;
}

void Enemy::resetShootCooldown() noexcept {
    m_currentShootCooldown = m_shootCooldown;
}

sf::Vector2f Enemy::getShootDirection(const sf::Vector2f& target) const noexcept {
    return Utils::normalize(target - m_position);
}

sf::Vector2f Enemy::getCenteredShootPosition(const sf::Vector2f& target) const noexcept {
    // m_position is already the center of the sprite (due to centered origin)
    sf::Vector2f centerPosition = m_position;
    
    // Calculate offset from sprite center to edge in shooting direction
    sf::Vector2f shootDirection = getShootDirection(target);
    
    // For sprites with centered origin, use actual sprite dimensions
    float spriteRadius;
    if (m_useSprites && m_sprite) {
        // Get the actual sprite bounds after scaling
        sf::FloatRect spriteBounds = m_sprite->getGlobalBounds();
        spriteRadius = std::max(spriteBounds.size.x, spriteBounds.size.y) * 0.5f;
    } else {
        // For geometric shape, use bounds
        sf::FloatRect bounds = getTightBounds();
        spriteRadius = std::max(bounds.size.x, bounds.size.y) * 0.5f;
    }
    
    // Position projectile just outside the sprite edge in shooting direction
    return centerPosition + shootDirection * (spriteRadius + 5.0f); // +5px buffer
}

void Enemy::takeDamage(float damage) noexcept {
    m_currentHealth = std::max(0.0f, m_currentHealth - damage);
    if (m_currentHealth <= 0.0f) {
        m_alive = false;
    }
}

int Enemy::getScoreValue() const noexcept {
    switch (m_type) {
        case EnemyType::BasicEnemy: return 10;
        case EnemyType::FastEnemy: return 20;
        case EnemyType::HeavyEnemy: return 30;
    }
    return 10;
}