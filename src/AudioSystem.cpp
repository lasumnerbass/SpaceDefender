#include "AudioSystem.hpp"
#include "Config.hpp"
#include <iostream>
#include <random>
#include <cmath>

AudioSystem::AudioSystem() {
    generateShootSound();
    generateExplosionSound();
    generateHitSound();

    // Initialize sound pool with dummy buffer (we'll set the proper buffer when playing)
    m_soundPool.reserve(20);
    for (size_t i = 0; i < 20; ++i) {
        m_soundPool.emplace_back(m_shootBuffer);
    }
}

void AudioSystem::playShoot() noexcept { 
    if (m_enabled) playSound(m_shootBuffer); 
}

void AudioSystem::playExplosion() noexcept { 
    if (m_enabled) playSound(m_explosionBuffer); 
}

void AudioSystem::playHit() noexcept { 
    if (m_enabled) playSound(m_hitBuffer); 
}

void AudioSystem::generateShootSound() {
    constexpr unsigned sampleRate = 44100;
    constexpr float duration = 0.1f;
    const size_t sampleCount = static_cast<size_t>(sampleRate * duration);
    std::vector<std::int16_t> samples(sampleCount);

    for (size_t i = 0; i < sampleCount; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float frequency = 800.0f * (1.0f - t * 2.0f);
        float amplitude = (1.0f - t) * 32767.0f;
        samples[i] = static_cast<std::int16_t>(amplitude * std::sin(Config::TwoPi * frequency * t));
    }

    std::vector<sf::SoundChannel> channelMap{sf::SoundChannel::Mono};
    bool result = m_shootBuffer.loadFromSamples(samples.data(), samples.size(),
                    1, sampleRate, channelMap);
    if (!result) std::cerr << "Warning: Failed to create shoot sound\n";
}

void AudioSystem::generateExplosionSound() {
    constexpr unsigned sampleRate = 44100;
    constexpr float duration = 0.3f;
    const size_t sampleCount = static_cast<size_t>(sampleRate * duration);
    std::vector<std::int16_t> samples(sampleCount);

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (size_t i = 0; i < sampleCount; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float envelope = std::exp(-t * 10.0f);
        float noise = dist(rng);
        samples[i] = static_cast<std::int16_t>(envelope * noise * 32767.0f);
    }

    std::vector<sf::SoundChannel> channelMap{sf::SoundChannel::Mono};
    bool result = m_explosionBuffer.loadFromSamples(samples.data(), samples.size(),
                    1, sampleRate, channelMap);
    if (!result) std::cerr << "Warning: Failed to create explosion sound\n";
}

void AudioSystem::generateHitSound() {
    constexpr unsigned sampleRate = 44100;
    constexpr float duration = 0.05f;
    const size_t sampleCount = static_cast<size_t>(sampleRate * duration);
    std::vector<std::int16_t> samples(sampleCount);

    for (size_t i = 0; i < sampleCount; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float frequency = 200.0f + 100.0f * std::sin(t * 50.0f);
        float amplitude = (1.0f - t * 2.0f) * 32767.0f;
        samples[i] = static_cast<std::int16_t>(amplitude * std::sin(Config::TwoPi * frequency * t));
    }

    std::vector<sf::SoundChannel> channelMap{sf::SoundChannel::Mono};
    bool result = m_hitBuffer.loadFromSamples(samples.data(), samples.size(),
                    1, sampleRate, channelMap);
    if (!result) std::cerr << "Warning: Failed to create hit sound\n";
}

void AudioSystem::playSound(const sf::SoundBuffer& buffer) noexcept {
    for (auto& sound : m_soundPool) {
        if (sound.getStatus() != sf::Sound::Status::Playing) {
            sound.setBuffer(buffer);
            sound.play();
            return;
        }
    }
}