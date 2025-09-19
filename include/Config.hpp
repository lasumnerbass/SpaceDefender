#pragma once

#include <iostream>

namespace Config {
    // Window Settings
    constexpr unsigned int WindowWidth = 1280;
    constexpr unsigned int WindowHeight = 960;
    constexpr unsigned int TargetFPS = 60;
    constexpr float MaxDeltaTime = 1.0f / 30.0f; // Prevent spiral of death
    
    // Math Constants
    constexpr float Pi = 3.14159265359f;
    constexpr float TwoPi = 2.0f * Pi;
    constexpr float HalfPi = Pi / 2.0f;
    
    // Player Settings
    constexpr float PlayerSpeed = 300.0f;
    constexpr float PlayerMaxHealth = 100.0f;
    constexpr float PlayerShootCooldown = 0.2f;
    constexpr float PlayerBoundsMargin = 20.0f;
    constexpr float PlayerDamage = 25.0f;
    
    // Enemy Settings
    constexpr size_t MaxEnemies = 3;
    constexpr float EnemySpawnCooldown = 2.0f;
    constexpr float EnemyBulletSpeed = 350.0f;
    constexpr float EnemyDamage = 10.0f;
    
    // Bullet Settings
    constexpr float BulletSpeed = 600.0f;
    constexpr size_t MaxBullets = 100;
    constexpr float PlayerBulletRadius = 4.0f;
    constexpr float EnemyBulletRadius = 3.0f;
    
    // Particle Settings
    constexpr size_t MaxParticles = 500;
    constexpr float ParticleLifetime = 1.0f;
    
    // Pool Sizes
    constexpr size_t BulletPoolSize = 50;
    constexpr size_t ParticlePoolSize = 200;
    constexpr size_t EnemyPoolSize = 10;
    
    // Performance Settings
    constexpr size_t FPSSampleSize = 60;
    constexpr float PerformanceUpdateInterval = 1.0f;
    
    // Sprite Sizing (all enemies same size as player)
    constexpr float PlayerSpriteSize = 64.0f;     // Base player size in pixels
    constexpr float BasicEnemySpriteSize = 64.0f; // Same as player
    constexpr float FastEnemySpriteSize = 64.0f;  // Same as player
    constexpr float HeavyEnemySpriteSize = 64.0f; // Same as player
    
    // Tight collision bounds (percentage of sprite size for tighter hit detection)
    constexpr float SpriteBoundsTightnessRatio = 0.6f; // Use 60% of sprite size for very tight collision
    constexpr float BulletBoundsTightnessRatio = 0.7f; // Use 70% of bullet size for tight collision
    
    // Sprite orientation offset (if sprites are not drawn pointing right by default)
    // Common values: 0.0f (right), -90.0f (up), 90.0f (down), 180.0f (left)
    constexpr float SpriteOrientationOffset = 90.0f; // Try pointing down offset
    
    // Debug Settings
    #ifdef DEBUG
    constexpr bool EnableAssertions = true;
    constexpr bool ShowDebugInfo = true;
    constexpr bool ShowDebugBoundaries = false; // Hide collision boundaries from user
    #else
    constexpr bool EnableAssertions = false;
    constexpr bool ShowDebugInfo = false;
    constexpr bool ShowDebugBoundaries = false; // Hide collision boundaries from user
    #endif
}

// Debug Macro
#define GAME_ASSERT(condition, message) \
    do { \
        if (Config::EnableAssertions && !(condition)) { \
            std::cerr << "Assertion failed: " << message << "\n" \
                     << "File: " << __FILE__ << "\n" \
                     << "Line: " << __LINE__ << std::endl; \
            std::abort(); \
        } \
    } while(0)