#include "PerformanceMonitor.hpp"
#include <algorithm>

void PerformanceMonitor::update(float deltaTime) noexcept {
    m_frameCount++;
    m_accumulatedTime += deltaTime;
    m_updateTimer += deltaTime;
    
    // Store frame time for rolling average
    m_frameTimes.push_back(deltaTime);
    if (m_frameTimes.size() > Config::FPSSampleSize) {
        m_frameTimes.pop_front();
    }
    
    // Update stats periodically
    if (m_updateTimer >= Config::PerformanceUpdateInterval) {
        m_currentFPS = deltaTime > 0.0f ? 1.0f / deltaTime : 0.0f;
        
        // Calculate average FPS
        float sum = 0.0f;
        for (float ft : m_frameTimes) {
            sum += ft;
        }
        m_averageFPS = m_frameTimes.empty() ? 0.0f :
                      m_frameTimes.size() / sum;
        
        // Update min/max
        m_minFPS = std::min(m_minFPS, m_currentFPS);
        m_maxFPS = std::max(m_maxFPS, m_currentFPS);
        
        m_updateTimer = 0.0f;
    }
}

void PerformanceMonitor::reset() noexcept {
    m_frameTimes.clear();
    m_accumulatedTime = 0.0f;
    m_updateTimer = 0.0f;
    m_currentFPS = 0.0f;
    m_averageFPS = 0.0f;
    m_minFPS = std::numeric_limits<float>::max();
    m_maxFPS = 0.0f;
    m_frameCount = 0;
}