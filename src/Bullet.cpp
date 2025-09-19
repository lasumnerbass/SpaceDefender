#include "Bullet.hpp"
#include "ResourceManager.hpp"
#include "Config.hpp"
#include "Utils.hpp"

void Bullet::setTexture(const sf::Texture& texture, const ResourceManager& resourceManager) {
    m_sprite = sf::Sprite(texture);
    
    // Scale bullet sprite to appropriate size (will be updated in initialize based on bullet type)
    float targetSize = 24.0f; // Default size, will be adjusted in initialize
    resourceManager.scaleTextureToExactSize(*m_sprite, targetSize, targetSize);
    
    m_useSprites = true;
}

void Bullet::enableSpriteMode(bool enable) {
    m_useSprites = enable;
}

void Bullet::initialize(const sf::Vector2f& position, const sf::Vector2f& direction, bool isPlayerBullet) noexcept {
    m_position = position;
    m_isPlayerBullet = isPlayerBullet;
    m_alive = true;
    
    m_damage = m_isPlayerBullet ? Config::PlayerDamage : Config::EnemyDamage;
    const float speed = m_isPlayerBullet ? Config::BulletSpeed : Config::EnemyBulletSpeed;
    m_velocity = Utils::normalize(direction) * speed;
    
    // Calculate rotation angle from direction vector
    m_rotation = Utils::radToDeg(std::atan2(direction.y, direction.x));
    
    // Set up geometric fallback
    const float radius = m_isPlayerBullet ? Config::PlayerBulletRadius : Config::EnemyBulletRadius;
    m_shape.setRadius(radius);
    m_shape.setFillColor(m_isPlayerBullet ? sf::Color::Yellow : sf::Color::Red);
    m_shape.setOrigin(sf::Vector2f(radius, radius));
    m_shape.setPosition(m_position);
    
    // Update sprite if available and resize appropriately
    if (m_sprite) {
        // Resize sprite based on bullet type
        float targetSize = m_isPlayerBullet ? Config::PlayerBulletRadius * 6.0f : Config::EnemyBulletRadius * 6.0f;
        
        // Get current texture size and calculate scale
        sf::Vector2u textureSize = m_sprite->getTexture().getSize();
        float scaleX = targetSize / static_cast<float>(textureSize.x);
        float scaleY = targetSize / static_cast<float>(textureSize.y);
        
        m_sprite->setScale(sf::Vector2f(scaleX, scaleY));
        m_sprite->setOrigin(sf::Vector2f(static_cast<float>(textureSize.x) / 2.0f, static_cast<float>(textureSize.y) / 2.0f));
        m_sprite->setPosition(m_position);
        m_sprite->setRotation(sf::degrees(m_rotation));
    }
}

void Bullet::update(float deltaTime) {
    if (!m_alive) return;
    
    m_position += m_velocity * deltaTime;
    
    // Update both representations
    m_shape.setPosition(m_position);
    if (m_sprite) {
        m_sprite->setPosition(m_position);
    }
    
    constexpr float margin = 50.0f;
    if (m_position.x < -margin || m_position.x > Config::WindowWidth + margin ||
        m_position.y < -margin || m_position.y > Config::WindowHeight + margin) {
        m_alive = false;
    }
}

void Bullet::draw(sf::RenderWindow& window) const {
    if (m_alive) {
        if (m_useSprites && m_sprite) {
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

void Bullet::drawDebugBounds(sf::RenderWindow& window) const noexcept {
    if (!m_alive) return;
    
    sf::Color outlineColor = m_isPlayerBullet ? 
        sf::Color(255, 255, 0, 192) :   // Bright yellow for player bullets
        sf::Color(255, 0, 0, 192);      // Bright red for enemy bullets
    
    // Draw only the tight bounds (actual collision bounds) for clarity
    sf::FloatRect tightBounds = getTightBounds();
    sf::RectangleShape tightDebugRect(tightBounds.size);
    tightDebugRect.setPosition(tightBounds.position);
    tightDebugRect.setFillColor(sf::Color::Transparent);
    tightDebugRect.setOutlineThickness(2.0f);
    tightDebugRect.setOutlineColor(outlineColor);
    window.draw(tightDebugRect);
}

sf::FloatRect Bullet::getBounds() const {
    // Use tight bounds for actual collision detection
    return getTightBounds();
}

sf::FloatRect Bullet::getTightBounds() const noexcept {
    sf::FloatRect originalBounds;
    if (m_useSprites && m_sprite) {
        originalBounds = m_sprite->getGlobalBounds();
    } else {
        originalBounds = m_shape.getGlobalBounds();
    }
    
    // Create tighter bounds by reducing size and centering
    sf::Vector2f originalSize = originalBounds.size;
    sf::Vector2f tightSize = originalSize * Config::BulletBoundsTightnessRatio;
    sf::Vector2f sizeReduction = originalSize - tightSize;
    sf::Vector2f positionOffset = sizeReduction * 0.5f; // Center the tight bounds
    
    return sf::FloatRect(originalBounds.position + positionOffset, tightSize);
}
