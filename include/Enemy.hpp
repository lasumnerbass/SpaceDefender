#pragma once

#include <SFML/Graphics.hpp>
#include <random>
#include <optional>
#include "GameObject.hpp"

class ResourceManager;

class Enemy : public GameObject {
private:
    sf::CircleShape m_shape;          // Keep for fallback
    std::optional<sf::Sprite> m_sprite;  // Sprite support (optional for SFML 3)
    bool m_useSprites = false;        // Toggle between modes
    float m_rotation = 0.0f;          // Sprite rotation angle
    
    EnemyType m_type = EnemyType::BasicEnemy;
    float m_maxHealth = 50.0f;
    float m_currentHealth = 50.0f;
    float m_shootCooldown = 2.0f;
    float m_currentShootCooldown = 0.0f;
    float m_moveTimer = 0.0f;
    sf::Vector2f m_targetDirection{1, 0};
    float m_speed = 100.0f;
    sf::Color m_baseColor = sf::Color::Magenta;
    
public:
    Enemy(); // Constructor for object pooling
    
    // Sprite methods
    void setTexture(const sf::Texture& texture, const ResourceManager& resourceManager);
    void enableSpriteMode(bool enable = true);
    [[nodiscard]] bool isUsingSpriteMode() const noexcept { return m_useSprites; }
    
    void initialize(const sf::Vector2f& position, EnemyType type, std::mt19937& rng) noexcept;
    void update(float deltaTime) override;
    void updateAI(std::mt19937& rng, float deltaTime) noexcept;
    void faceTarget(const sf::Vector2f& targetPosition) noexcept;
    void draw(sf::RenderWindow& window) const override;
    void drawDebugBounds(sf::RenderWindow& window) const noexcept;
    [[nodiscard]] sf::FloatRect getBounds() const override;
    [[nodiscard]] sf::FloatRect getTightBounds() const noexcept;
    
    [[nodiscard]] bool canShoot() const noexcept;
    void resetShootCooldown() noexcept;
    [[nodiscard]] sf::Vector2f getShootDirection(const sf::Vector2f& target) const noexcept;
    [[nodiscard]] sf::Vector2f getCenteredShootPosition(const sf::Vector2f& target) const noexcept;
    
    void takeDamage(float damage) noexcept;
    [[nodiscard]] const sf::Color& getColor() const noexcept { return m_baseColor; }
    [[nodiscard]] int getScoreValue() const noexcept;
    [[nodiscard]] EnemyType getType() const noexcept { return m_type; }
};