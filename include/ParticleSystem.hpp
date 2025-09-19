#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include "Config.hpp"

class ParticleSystem {
private:
    struct ParticleData {
        std::vector<sf::Vector2f> positions;
        std::vector<sf::Vector2f> velocities;
        std::vector<sf::Color> colors;
        std::vector<float> lifetimes;
        std::vector<float> maxLifetimes;
        size_t activeCount = 0;
    } m_particles;
    
    sf::VertexArray m_vertices;
    std::mt19937& m_rng;
    std::uniform_real_distribution<float> m_angleDist{0, Config::TwoPi};
    std::uniform_real_distribution<float> m_speedDist{100.0f, 300.0f};
    
public:
    explicit ParticleSystem(std::mt19937& rng);
    
    void emit(const sf::Vector2f& position, const sf::Color& color, int count) noexcept;
    void update(float deltaTime) noexcept;
    void draw(sf::RenderWindow& window) const noexcept;
    void clear() noexcept;
    
    [[nodiscard]] size_t getActiveCount() const noexcept { return m_particles.activeCount; }
};