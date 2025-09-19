#pragma once

#include <SFML/Graphics.hpp>
#include <optional>
#include "GameObject.hpp"
#include "InputManager.hpp"

class ResourceManager;

class Player : public GameObject {
private:
    static constexpr float ShipOffset = 30.0f;
    
    sf::ConvexShape m_shape;          // Keep for fallback
    std::optional<sf::Sprite> m_sprite;  // Sprite support (optional for SFML 3)
    bool m_useSprites = false;        // Toggle between modes
    
    float m_currentHealth = 100.0f;
    float m_currentShootCooldown = 0.0f;
    bool m_wantToShoot = false;
    
public:
    Player();
    
    // Sprite methods
    void setTexture(const sf::Texture& texture, const ResourceManager& resourceManager);
    void enableSpriteMode(bool enable = true);
    [[nodiscard]] bool isUsingSpriteMode() const noexcept { return m_useSprites; }
    
    void initialize(const sf::Vector2f& position) noexcept;
    void update(float deltaTime) override;
    void updateMovement(const InputManager& input, float deltaTime) noexcept;
    void draw(sf::RenderWindow& window) const override;
    void drawHealthBar(sf::RenderWindow& window) const noexcept;
    void drawDebugBounds(sf::RenderWindow& window) const noexcept;
    [[nodiscard]] sf::FloatRect getBounds() const override;
    [[nodiscard]] sf::FloatRect getTightBounds() const noexcept;
    
    [[nodiscard]] bool canShoot() const noexcept;
    void setWantToShoot(bool wantToShoot) noexcept { m_wantToShoot = wantToShoot; }
    [[nodiscard]] bool wantsToShoot() const noexcept { return m_wantToShoot; }
    void resetShootCooldown() noexcept;
    
    [[nodiscard]] sf::Vector2f getShootDirection() const noexcept;
    [[nodiscard]] sf::Vector2f getShootPosition() const noexcept;
    [[nodiscard]] sf::Vector2f getCenteredShootPosition() const noexcept;
    
    void takeDamage(float damage) noexcept;
    [[nodiscard]] float getHealth() const noexcept { return m_currentHealth; }
};