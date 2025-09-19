#include "InputManager.hpp"

void InputManager::update() noexcept {
    m_keysJustPressed.clear();
    m_keysJustReleased.clear();
    m_mouseJustPressed.clear();
}

void InputManager::keyPressed(sf::Keyboard::Key key) noexcept {
    if (m_keysPressed.find(key) == m_keysPressed.end()) {
        m_keysJustPressed.insert(key);
    }
    m_keysPressed.insert(key);
}

void InputManager::keyReleased(sf::Keyboard::Key key) noexcept {
    m_keysPressed.erase(key);
    m_keysJustReleased.insert(key);
}

void InputManager::mousePressed(sf::Mouse::Button button) noexcept {
    if (m_mousePressed.find(button) == m_mousePressed.end()) {
        m_mouseJustPressed.insert(button);
    }
    m_mousePressed.insert(button);
}

void InputManager::mouseReleased(sf::Mouse::Button button) noexcept {
    m_mousePressed.erase(button);
}

void InputManager::clearAll() noexcept {
    m_keysPressed.clear();
    m_keysJustPressed.clear();
    m_keysJustReleased.clear();
    m_mousePressed.clear();
    m_mouseJustPressed.clear();
}

bool InputManager::isKeyPressed(sf::Keyboard::Key key) const noexcept {
    return m_keysPressed.count(key) > 0;
}

bool InputManager::isKeyJustPressed(sf::Keyboard::Key key) const noexcept {
    return m_keysJustPressed.count(key) > 0;
}

bool InputManager::isKeyJustReleased(sf::Keyboard::Key key) const noexcept {
    return m_keysJustReleased.count(key) > 0;
}

bool InputManager::isMousePressed(sf::Mouse::Button button) const noexcept {
    return m_mousePressed.count(button) > 0;
}

bool InputManager::isMouseJustPressed(sf::Mouse::Button button) const noexcept {
    return m_mouseJustPressed.count(button) > 0;
}