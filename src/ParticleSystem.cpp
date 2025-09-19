#include "ParticleSystem.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <algorithm>

ParticleSystem::ParticleSystem(std::mt19937& rng) : m_rng(rng) {
    m_particles.positions.reserve(Config::MaxParticles);
    m_particles.velocities.reserve(Config::MaxParticles);
    m_particles.colors.reserve(Config::MaxParticles);
    m_particles.lifetimes.reserve(Config::MaxParticles);
    m_particles.maxLifetimes.reserve(Config::MaxParticles);
    
    m_vertices.setPrimitiveType(sf::PrimitiveType::Points);
    m_vertices.resize(Config::MaxParticles);
}

void ParticleSystem::emit(const sf::Vector2f& position, const sf::Color& color, int count) noexcept {
    const size_t available = Config::MaxParticles - m_particles.activeCount;
    const size_t toAdd = std::min(static_cast<size_t>(count), available);
    
    for (size_t i = 0; i < toAdd; ++i) {
        if (m_particles.activeCount >= Config::MaxParticles) break;
        
        const float angle = m_angleDist(m_rng);
        const float speed = m_speedDist(m_rng);
        const sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);
        
        if (m_particles.activeCount < m_particles.positions.size()) {
            m_particles.positions[m_particles.activeCount] = position;
            m_particles.velocities[m_particles.activeCount] = velocity;
            m_particles.colors[m_particles.activeCount] = color;
            m_particles.lifetimes[m_particles.activeCount] = 0.0f;
            m_particles.maxLifetimes[m_particles.activeCount] = Config::ParticleLifetime;
        } else {
            m_particles.positions.push_back(position);
            m_particles.velocities.push_back(velocity);
            m_particles.colors.push_back(color);
            m_particles.lifetimes.push_back(0.0f);
            m_particles.maxLifetimes.push_back(Config::ParticleLifetime);
        }
        m_particles.activeCount++;
    }
}

void ParticleSystem::update(float deltaTime) noexcept {
    size_t writeIndex = 0;
    
    for (size_t i = 0; i < m_particles.activeCount; ++i) {
        m_particles.lifetimes[i] += deltaTime;
        
        if (m_particles.lifetimes[i] < m_particles.maxLifetimes[i]) {
            m_particles.positions[i] += m_particles.velocities[i] * deltaTime;
            
            const float lifeRatio = m_particles.lifetimes[i] / m_particles.maxLifetimes[i];
            sf::Color color = m_particles.colors[i];
            color.a = static_cast<std::uint8_t>(255 * (1.0f - lifeRatio));
            m_particles.colors[i] = color;
            
            if (writeIndex != i) {
                m_particles.positions[writeIndex] = m_particles.positions[i];
                m_particles.velocities[writeIndex] = m_particles.velocities[i];
                m_particles.colors[writeIndex] = m_particles.colors[i];
                m_particles.lifetimes[writeIndex] = m_particles.lifetimes[i];
                m_particles.maxLifetimes[writeIndex] = m_particles.maxLifetimes[i];
            }
            writeIndex++;
        }
    }
    
    m_particles.activeCount = writeIndex;
}

void ParticleSystem::draw(sf::RenderWindow& window) const noexcept {
    for (size_t i = 0; i < m_particles.activeCount; ++i) {
        sf::CircleShape particle(2.0f);
        particle.setPosition(m_particles.positions[i] - sf::Vector2f(2.0f, 2.0f));
        particle.setFillColor(m_particles.colors[i]);
        window.draw(particle);
    }
}

void ParticleSystem::clear() noexcept {
    m_particles.activeCount = 0;
}