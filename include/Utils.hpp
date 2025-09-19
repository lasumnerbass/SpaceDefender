#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <cmath>
#include "Config.hpp"

namespace Utils {
    [[nodiscard]] inline constexpr float distanceSquared(const sf::Vector2f& a, const sf::Vector2f& b) noexcept;
    [[nodiscard]] inline float distance(const sf::Vector2f& a, const sf::Vector2f& b) noexcept;
    [[nodiscard]] inline float vectorLength(const sf::Vector2f& vec) noexcept;
    [[nodiscard]] inline constexpr float vectorLengthSquared(const sf::Vector2f& vec) noexcept;
    [[nodiscard]] inline sf::Vector2f normalize(const sf::Vector2f& vec) noexcept;
    [[nodiscard]] inline constexpr float radToDeg(float radians) noexcept;
    [[nodiscard]] inline constexpr float degToRad(float degrees) noexcept;
    [[nodiscard]] inline constexpr float lerp(float a, float b, float t) noexcept;
    [[nodiscard]] inline constexpr float clamp(float value, float min, float max) noexcept;
    [[nodiscard]] inline bool rectIntersects(const sf::FloatRect& rect1, const sf::FloatRect& rect2) noexcept;
}

// Inline implementations
namespace Utils {
    inline constexpr float distanceSquared(const sf::Vector2f& a, const sf::Vector2f& b) noexcept {
        const float dx = b.x - a.x;
        const float dy = b.y - a.y;
        return dx * dx + dy * dy;
    }
    
    inline float distance(const sf::Vector2f& a, const sf::Vector2f& b) noexcept {
        return std::sqrt(distanceSquared(a, b));
    }
    
    inline float vectorLength(const sf::Vector2f& vec) noexcept {
        return std::sqrt(vec.x * vec.x + vec.y * vec.y);
    }
    
    inline constexpr float vectorLengthSquared(const sf::Vector2f& vec) noexcept {
        return vec.x * vec.x + vec.y * vec.y;
    }
    
    inline sf::Vector2f normalize(const sf::Vector2f& vec) noexcept {
        const float length = vectorLength(vec);
        if (length > 0.0f) {
            const float invLength = 1.0f / length;
            return sf::Vector2f(vec.x * invLength, vec.y * invLength);
        }
        return vec;
    }
    
    inline constexpr float radToDeg(float radians) noexcept {
        return radians * 180.0f / Config::Pi;
    }
    
    inline constexpr float degToRad(float degrees) noexcept {
        return degrees * Config::Pi / 180.0f;
    }
    
    inline constexpr float lerp(float a, float b, float t) noexcept {
        return a + (b - a) * t;
    }
    
    inline constexpr float clamp(float value, float min, float max) noexcept {
        return value < min ? min : (value > max ? max : value);
    }
    
    inline bool rectIntersects(const sf::FloatRect& rect1, const sf::FloatRect& rect2) noexcept {
        const float r1Left = rect1.position.x;
        const float r1Top = rect1.position.y;
        const float r1Right = rect1.position.x + rect1.size.x;
        const float r1Bottom = rect1.position.y + rect1.size.y;
        
        const float r2Left = rect2.position.x;
        const float r2Top = rect2.position.y;
        const float r2Right = rect2.position.x + rect2.size.x;
        const float r2Bottom = rect2.position.y + rect2.size.y;
        
        return r1Left < r2Right && r1Right > r2Left &&
               r1Top < r2Bottom && r1Bottom > r2Top;
    }
}