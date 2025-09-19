#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include "Config.hpp"

template<typename T>
class ObjectPool {
public:
    struct Statistics {
        size_t totalAllocated = 0;
        size_t currentActive = 0;
        size_t peakActive = 0;
        size_t totalAcquired = 0;
        size_t totalReleased = 0;
    };
    
private:
    std::vector<std::unique_ptr<T>> m_allObjects;
    std::vector<T*> m_available;
    Statistics m_stats;
    
public:
    void initialize(size_t count) {
        GAME_ASSERT(count > 0, "Pool size must be greater than 0");
        
        m_allObjects.reserve(count);
        m_available.reserve(count);
        
        for (size_t i = 0; i < count; ++i) {
            auto obj = std::make_unique<T>();
            m_available.push_back(obj.get());
            m_allObjects.push_back(std::move(obj));
        }
        
        m_stats.totalAllocated = count;
    }
    
    [[nodiscard]] T* acquire() noexcept {
        if (!m_available.empty()) {
            T* obj = m_available.back();
            m_available.pop_back();
            
            m_stats.currentActive++;
            m_stats.totalAcquired++;
            m_stats.peakActive = std::max(m_stats.peakActive, m_stats.currentActive);
            
            return obj;
        }
        return nullptr;
    }
    
    void release(T* obj) noexcept {
        if (obj) {
            obj->reset();
            m_available.push_back(obj);
            
            if (m_stats.currentActive > 0) {
                m_stats.currentActive--;
            }
            m_stats.totalReleased++;
        }
    }
    
    void releaseAll() noexcept {
        m_available.clear();
        for (auto& obj : m_allObjects) {
            obj->reset();
            m_available.push_back(obj.get());
        }
        m_stats.currentActive = 0;
    }
    
    [[nodiscard]] const Statistics& getStats() const noexcept { return m_stats; }
    [[nodiscard]] size_t getAvailableCount() const noexcept { return m_available.size(); }
    [[nodiscard]] float getUsageRatio() const noexcept {
        return m_stats.totalAllocated > 0 ?
               static_cast<float>(m_stats.currentActive) / static_cast<float>(m_stats.totalAllocated) : 0.0f;
    }
};