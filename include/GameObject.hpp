#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

enum class GameState {
    MainMenu,
    Playing,
    GameOver,
    Paused
};

enum class EnemyType {
    BasicEnemy,
    FastEnemy,
    HeavyEnemy
};

class GameObject {
protected:
    sf::Vector2f m_position{0, 0};
    sf::Vector2f m_velocity{0, 0};
    float m_rotation = 0.0f;
    bool m_alive = false;
    
public:
    virtual ~GameObject() = default;
    
    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;
    [[nodiscard]] virtual sf::FloatRect getBounds() const = 0;
    
    virtual void reset() noexcept;
    
    [[nodiscard]] bool isAlive() const noexcept { return m_alive; }
    void setAlive(bool state) noexcept { m_alive = state; }
    [[nodiscard]] const sf::Vector2f& getPosition() const noexcept { return m_position; }
    void setPosition(const sf::Vector2f& position) noexcept { m_position = position; }
};