#pragma once

#include <SFML/Audio.hpp>
#include <vector>

class AudioSystem {
private:
    sf::SoundBuffer m_shootBuffer;
    sf::SoundBuffer m_explosionBuffer;
    sf::SoundBuffer m_hitBuffer;
    std::vector<sf::Sound> m_soundPool;
    bool m_enabled = true;

public:
    AudioSystem();

    void playShoot() noexcept;
    void playExplosion() noexcept;
    void playHit() noexcept;

    void setEnabled(bool enabled) noexcept { m_enabled = enabled; }
    [[nodiscard]] bool isEnabled() const noexcept { return m_enabled; }

private:
    void generateShootSound();
    void generateExplosionSound();
    void generateHitSound();
    void playSound(const sf::SoundBuffer& buffer) noexcept;
};