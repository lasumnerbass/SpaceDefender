#pragma once

#include <SFML/Graphics.hpp>
#include <optional>
#include "GameObject.hpp"

class ResourceManager;

class Bullet : public GameObject {
private:
    sf::CircleShape m_shape;          // Keep for fallback
    std::optional<sf::Sprite> m_sprite;  // Sprite support
    bool m_useSprites = false;        // Toggle between modes
    float m_rotation = 0.0f;          // Sprite rotation angle
    
    bool m_isPlayerBullet = true;
    float m_damage = 25.0f;
    
public:
    // Sprite methods
    void setTexture(const sf::Texture& texture, const ResourceManager& resourceManager);
    void enableSpriteMode(bool enable = true);
    [[nodiscard]] bool isUsingSpriteMode() const noexcept { return m_useSprites; }
    
    void initialize(const sf::Vector2f& position, const sf::Vector2f& direction, bool isPlayerBullet) noexcept;
    
    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) const override;
    void drawDebugBounds(sf::RenderWindow& window) const noexcept;
    [[nodiscard]] sf::FloatRect getBounds() const override;
    [[nodiscard]] sf::FloatRect getTightBounds() const noexcept;
    
    [[nodiscard]] bool isPlayerBullet() const noexcept { return m_isPlayerBullet; }
    [[nodiscard]] float getDamage() const noexcept { return m_damage; }
};