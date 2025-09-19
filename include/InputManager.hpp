#pragma once

#include <unordered_set>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

class InputManager {
private:
    std::unordered_set<sf::Keyboard::Key> m_keysPressed;
    std::unordered_set<sf::Keyboard::Key> m_keysJustPressed;
    std::unordered_set<sf::Keyboard::Key> m_keysJustReleased;
    std::unordered_set<sf::Mouse::Button> m_mousePressed;
    std::unordered_set<sf::Mouse::Button> m_mouseJustPressed;
    
public:
    void update() noexcept;
    void keyPressed(sf::Keyboard::Key key) noexcept;
    void keyReleased(sf::Keyboard::Key key) noexcept;
    void mousePressed(sf::Mouse::Button button) noexcept;
    void mouseReleased(sf::Mouse::Button button) noexcept;
    void clearAll() noexcept;
    
    [[nodiscard]] bool isKeyPressed(sf::Keyboard::Key key) const noexcept;
    [[nodiscard]] bool isKeyJustPressed(sf::Keyboard::Key key) const noexcept;
    [[nodiscard]] bool isKeyJustReleased(sf::Keyboard::Key key) const noexcept;
    [[nodiscard]] bool isMousePressed(sf::Mouse::Button button) const noexcept;
    [[nodiscard]] bool isMouseJustPressed(sf::Mouse::Button button) const noexcept;
};