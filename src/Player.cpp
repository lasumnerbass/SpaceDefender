#include "Player.hpp"
#include "ResourceManager.hpp"
#include "Config.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <cmath>

Player::Player() {
    // Keep existing geometric setup as fallback
    m_shape.setPointCount(3);
    m_shape.setPoint(0, sf::Vector2f(20.0f, 0.0f));
    m_shape.setPoint(1, sf::Vector2f(-10.0f, -10.0f));
    m_shape.setPoint(2, sf::Vector2f(-10.0f, 10.0f));
    m_shape.setFillColor(sf::Color::Cyan);
    m_shape.setOutlineThickness(2.0f);
    m_shape.setOutlineColor(sf::Color::White);
    m_shape.setOrigin(sf::Vector2f(5.0f, 0.0f));
    
    // Initialize sprite when texture is set
}

void Player::setTexture(const sf::Texture& texture, const ResourceManager& resourceManager) {
    m_sprite = sf::Sprite(texture);
    
    // Use exact sizing to force all sprites to identical pixel dimensions
    resourceManager.scaleTextureToExactSize(*m_sprite, Config::PlayerSpriteSize, Config::PlayerSpriteSize);
    
    m_useSprites = true;
}

void Player::enableSpriteMode(bool enable) {
    m_useSprites = enable;
}

void Player::initialize(const sf::Vector2f& position) noexcept {
    m_position = position;
    m_alive = true;
    m_currentHealth = Config::PlayerMaxHealth;
    m_currentShootCooldown = 0.0f;
    m_wantToShoot = false;
    m_shape.setPosition(m_position);
}

void Player::update(float deltaTime) {
    m_currentShootCooldown = std::max(0.0f, m_currentShootCooldown - deltaTime);
}

void Player::updateMovement(const InputManager& input, float deltaTime) noexcept {
    sf::Vector2f movement(0.0f, 0.0f);
    
    if (input.isKeyPressed(sf::Keyboard::Key::Left) ||
        input.isKeyPressed(sf::Keyboard::Key::A)) {
        movement.x = -Config::PlayerSpeed;
    }
    if (input.isKeyPressed(sf::Keyboard::Key::Right) ||
        input.isKeyPressed(sf::Keyboard::Key::D)) {
        movement.x = Config::PlayerSpeed;
    }
    if (input.isKeyPressed(sf::Keyboard::Key::Up) ||
        input.isKeyPressed(sf::Keyboard::Key::W)) {
        movement.y = -Config::PlayerSpeed;
    }
    if (input.isKeyPressed(sf::Keyboard::Key::Down) ||
        input.isKeyPressed(sf::Keyboard::Key::S)) {
        movement.y = Config::PlayerSpeed;
    }
    
    if (movement.x != 0 && movement.y != 0) {
        movement = Utils::normalize(movement) * Config::PlayerSpeed;
    }
    
    m_position += movement * deltaTime;
    m_position.x = Utils::clamp(m_position.x, Config::PlayerBoundsMargin,
                                Config::WindowWidth - Config::PlayerBoundsMargin);
    m_position.y = Utils::clamp(m_position.y, Config::PlayerBoundsMargin,
                                Config::WindowHeight - Config::PlayerBoundsMargin);
    
    if (movement.x != 0 || movement.y != 0) {
        m_rotation = Utils::radToDeg(std::atan2(movement.y, movement.x));
    }
    
    // Update both representations
    m_shape.setPosition(m_position);
    m_shape.setRotation(sf::degrees(m_rotation));
    
    if (m_sprite) {
        m_sprite->setPosition(m_position);
        // Player sprite faces the movement/shooting direction (with orientation offset)
        m_sprite->setRotation(sf::degrees(m_rotation + Config::SpriteOrientationOffset));
    }
}

void Player::draw(sf::RenderWindow& window) const {
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

void Player::drawHealthBar(sf::RenderWindow& window) const noexcept {
    sf::RectangleShape bgBar(sf::Vector2f(200.0f, 20.0f));
    bgBar.setPosition(sf::Vector2f(10.0f, 10.0f));
    bgBar.setFillColor(sf::Color(50, 50, 50));
    bgBar.setOutlineThickness(2.0f);
    bgBar.setOutlineColor(sf::Color::White);
    window.draw(bgBar);
    
    const float healthPercentage = m_currentHealth / Config::PlayerMaxHealth;
    sf::RectangleShape healthBar(sf::Vector2f(196.0f * healthPercentage, 16.0f));
    healthBar.setPosition(sf::Vector2f(12.0f, 12.0f));
    
    if (healthPercentage > 0.6f) {
        healthBar.setFillColor(sf::Color::Green);
    } else if (healthPercentage > 0.3f) {
        healthBar.setFillColor(sf::Color::Yellow);
    } else {
        healthBar.setFillColor(sf::Color::Red);
    }
    window.draw(healthBar);
}

void Player::drawDebugBounds(sf::RenderWindow& window) const noexcept {
    // Draw only the tight bounds (actual collision bounds) for clarity
    sf::FloatRect tightBounds = getTightBounds();
    sf::RectangleShape tightDebugRect(tightBounds.size);
    tightDebugRect.setPosition(tightBounds.position);
    tightDebugRect.setFillColor(sf::Color::Transparent);
    tightDebugRect.setOutlineThickness(2.0f);
    tightDebugRect.setOutlineColor(sf::Color(0, 255, 0, 192)); // Bright green for player
    window.draw(tightDebugRect);
}

sf::FloatRect Player::getBounds() const {
    // Use tight bounds for actual collision detection
    return getTightBounds();
}

sf::FloatRect Player::getTightBounds() const noexcept {
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

bool Player::canShoot() const noexcept {
    return m_currentShootCooldown <= 0.0f;
}

void Player::resetShootCooldown() noexcept {
    m_currentShootCooldown = Config::PlayerShootCooldown;
}

sf::Vector2f Player::getShootDirection() const noexcept {
    return sf::Vector2f(std::cos(Utils::degToRad(m_rotation)),
                       std::sin(Utils::degToRad(m_rotation)));
}

sf::Vector2f Player::getShootPosition() const noexcept {
    return getCenteredShootPosition();
}

sf::Vector2f Player::getCenteredShootPosition() const noexcept {
    // m_position is already the center of the sprite (due to centered origin)
    sf::Vector2f centerPosition = m_position;
    
    // Calculate offset from sprite center to edge in shooting direction
    sf::Vector2f shootDirection = getShootDirection();
    
    // For sprites with centered origin, use actual sprite dimensions
    float spriteRadius;
    if (m_useSprites && m_sprite) {
        // Get the actual sprite bounds after scaling
        sf::FloatRect spriteBounds = m_sprite->getGlobalBounds();
        spriteRadius = std::max(spriteBounds.size.x, spriteBounds.size.y) * 0.5f;
        
        // Account for sprite orientation offset when calculating spawn position
        // The sprite visual orientation differs from the logical shooting direction
        float visualRotation = m_rotation + Config::SpriteOrientationOffset;
        sf::Vector2f visualShootDirection = sf::Vector2f(
            std::cos(Utils::degToRad(visualRotation)),
            std::sin(Utils::degToRad(visualRotation))
        );
        
        // Use the visual direction for projectile spawn positioning
        return centerPosition + visualShootDirection * (spriteRadius + 5.0f);
    } else {
        // For geometric shape, use the logical shooting direction
        sf::FloatRect bounds = getTightBounds();
        spriteRadius = std::max(bounds.size.x, bounds.size.y) * 0.5f;
        return centerPosition + shootDirection * (spriteRadius + 5.0f);
    }
}

void Player::takeDamage(float damage) noexcept {
    m_currentHealth = std::max(0.0f, m_currentHealth - damage);
    if (m_currentHealth <= 0.0f) {
        m_alive = false;
    }
}