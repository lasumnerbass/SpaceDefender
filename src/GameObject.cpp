#include "GameObject.hpp"

void GameObject::reset() noexcept {
    m_alive = false;
    m_velocity = sf::Vector2f(0, 0);
    m_position = sf::Vector2f(0, 0);
    m_rotation = 0.0f;
}