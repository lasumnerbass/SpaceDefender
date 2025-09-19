#pragma once

#include <deque>
#include <limits>
#include "Config.hpp"

class PerformanceMonitor {
private:
    std::deque<float> m_frameTimes;
    float m_accumulatedTime = 0.0f;
    float m_updateTimer = 0.0f;
    float m_currentFPS = 0.0f;
    float m_averageFPS = 0.0f;
    float m_minFPS = std::numeric_limits<float>::max();
    float m_maxFPS = 0.0f;
    size_t m_frameCount = 0;
    
public:
    void update(float deltaTime) noexcept;
    void reset() noexcept;
    
    [[nodiscard]] float getCurrentFPS() const noexcept { return m_currentFPS; }
    [[nodiscard]] float getAverageFPS() const noexcept { return m_averageFPS; }
    [[nodiscard]] float getMinFPS() const noexcept { return m_minFPS; }
    [[nodiscard]] float getMaxFPS() const noexcept { return m_maxFPS; }
    [[nodiscard]] size_t getFrameCount() const noexcept { return m_frameCount; }
};