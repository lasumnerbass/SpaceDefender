#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <optional>

class ResourceManager;

/**
 * @brief Handles animated explosion effects using sprite sheet frames
 */
class ExplosionAnimation {
public:
    /**
     * @brief Different explosion types for different objects
     */
    enum class ExplosionType {
        Small,   // For bullets and small objects
        Medium,  // For basic enemies  
        Large    // For player and heavy enemies
    };

private:
    sf::Vector2f m_position;
    ExplosionType m_type;
    std::optional<sf::Sprite> m_sprite;
    
    // Animation state
    bool m_active;
    float m_frameTime;
    float m_currentFrameTime;
    int m_currentFrame;
    int m_totalFrames;
    
    // Sprite sheet configuration
    int m_framesPerRow;
    int m_frameWidth;
    int m_frameHeight;
    
    // Scale factor based on explosion type
    float m_scale;

public:
    ExplosionAnimation();
    ~ExplosionAnimation() = default;
    
    /**
     * @brief Initialize explosion at given position
     * @param position World position for explosion center
     * @param type Type of explosion (affects size and duration)
     * @param texture Explosion sprite sheet texture
     */
    void initialize(const sf::Vector2f& position, ExplosionType type, const sf::Texture& texture);
    
    /**
     * @brief Update animation frame
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime);
    
    /**
     * @brief Draw explosion animation
     * @param window Render window
     */
    void draw(sf::RenderWindow& window) const;
    
    /**
     * @brief Check if explosion animation is still active
     * @return True if animation is playing, false if finished
     */
    bool isActive() const noexcept { return m_active; }
    
    /**
     * @brief Reset explosion to inactive state
     */
    void reset();
    
private:
    /**
     * @brief Set up sprite sheet frame based on current frame
     */
    void updateSpriteFrame();
    
    /**
     * @brief Configure explosion parameters based on type
     * @param type Explosion type
     */
    void configureForType(ExplosionType type);
};

/**
 * @brief Manager class for multiple explosion animations
 */
class ExplosionManager {
private:
    std::vector<std::unique_ptr<ExplosionAnimation>> m_explosions;
    const sf::Texture* m_explosionTexture;
    
public:
    ExplosionManager();
    ~ExplosionManager() = default;
    
    /**
     * @brief Initialize with explosion texture from resource manager
     * @param resourceManager Resource manager containing explosion texture
     */
    void initialize(const ResourceManager& resourceManager);
    
    /**
     * @brief Trigger an explosion at the specified position
     * @param position World position for explosion
     * @param type Type of explosion
     */
    void createExplosion(const sf::Vector2f& position, ExplosionAnimation::ExplosionType type);
    
    /**
     * @brief Update all active explosions
     * @param deltaTime Time elapsed since last update
     */
    void update(float deltaTime);
    
    /**
     * @brief Draw all active explosions
     * @param window Render window
     */
    void draw(sf::RenderWindow& window) const;
    
    /**
     * @brief Clean up finished explosions
     */
    void cleanup();
    
private:
    /**
     * @brief Find an inactive explosion for reuse, or create new one
     * @return Pointer to available explosion animation
     */
    ExplosionAnimation* getAvailableExplosion();
};