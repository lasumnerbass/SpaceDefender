#include "ResourceManager.hpp"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <array>
#include <vector>
#include <iostream>
#include <random>
#include <filesystem>

std::string ResourceManager::findProjectRoot() const {
    // Look for project indicators starting from current directory
    std::filesystem::path current = std::filesystem::current_path();
    
    // Look for specific project files that indicate the root
    std::vector<std::string> indicators = {
        "resources",
        "CMakeLists.txt", 
        "SpaceDefender.xcodeproj",
        "README.md"
    };
    
    // Search up the directory tree
    for (int levels = 0; levels < 5; ++levels) {
        for (const auto& indicator : indicators) {
            if (std::filesystem::exists(current / indicator)) {
                return current.string();
            }
        }
        current = current.parent_path();
        if (current.empty() || current == current.parent_path()) {
            break;
        }
    }
    
    // Fallback to hardcoded path
    return "/Users/courtneybass/MSD/Day_22/SpaceDefender";
}

void ResourceManager::scaleTextureToExactSize(sf::Sprite& sprite, float targetWidth, float targetHeight) const {
    // Get the original texture bounds
    sf::FloatRect originalBounds = sprite.getLocalBounds();
    
    // Calculate exact scaling factors for width and height independently
    float scaleX = targetWidth / originalBounds.size.x;
    float scaleY = targetHeight / originalBounds.size.y;
    
    // Apply the scaling to force exact dimensions
    sprite.setScale({scaleX, scaleY});
    
    // Center the sprite origin for the new size
    sprite.setOrigin({targetWidth / 2.0f, targetHeight / 2.0f});
    
    // Sprite scaled to exact dimensions
}

bool ResourceManager::loadSystemFont() {
    const std::array<std::string, 6> fontPaths = {{
        "/System/Library/Fonts/Helvetica.ttc",
        "/System/Library/Fonts/Avenir.ttc",
        "/System/Library/Fonts/Arial.ttf",
        "/Library/Fonts/Arial.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Courier.ttc"
    }};
    
    for (const auto& path : fontPaths) {
        if (m_font.openFromFile(path)) {
            std::cout << "Font loaded from: " << path << std::endl;
            m_fontLoaded = true;
            return true;
        }
    }
    
    std::cout << "Warning: Could not load system font" << std::endl;
    return false;
}

bool ResourceManager::loadTexture(const std::string& name, const std::string& filePath) {
    sf::Texture texture;
    
    // Get the project root directory
    std::string projectRoot = findProjectRoot();
    std::string absolutePath = projectRoot + "/" + filePath;
    
    // Try the smart absolute path first, then fallbacks
    std::vector<std::string> possiblePaths = {
        absolutePath,                               // Smart absolute path from project root
        filePath,                                   // Original relative path
        "../" + filePath,                          // One level up (common for Xcode)
        "../../" + filePath,                       // Two levels up
        "../../../" + filePath,                    // Three levels up
    };
    
    for (const auto& path : possiblePaths) {
        if (texture.loadFromFile(path)) {
            texture.setSmooth(true); // Enable texture filtering
            m_textures[name] = std::move(texture);
            std::cout << "Loaded texture: " << name << " from " << path << std::endl;
            return true;
        }
    }
    
    std::cerr << "Failed to load texture: " << name << " (tried " << possiblePaths.size() << " paths)" << std::endl;
    std::cerr << "Project root detected as: " << projectRoot << std::endl;
    return false;
}

bool ResourceManager::hasTexture(const std::string& name) const noexcept {
    return m_textures.find(name) != m_textures.end();
}

const sf::Texture& ResourceManager::getTexture(const std::string& name) const {
    auto it = m_textures.find(name);
    if (it == m_textures.end()) {
        throw std::runtime_error("Texture not found: " + name);
    }
    return it->second;
}

void ResourceManager::loadAllGameTextures() {
    std::cout << "Loading game textures..." << std::endl;
    
    // Try to load sprite assets, continue gracefully if not found
    loadTexture("player_ship", "resources/textures/ships/player_ship.png");
    loadTexture("enemy_basic", "resources/textures/ships/enemy_basic.png");
    loadTexture("enemy_fast", "resources/textures/ships/enemy_fast.png");
    loadTexture("enemy_heavy", "resources/textures/ships/enemy_heavy.png");
    
    // Try to load explosion sprite sheet
    loadTexture("explosion", "resources/textures/effects/explosion.png");
    
    // Try to load bullet sprites
    loadTexture("player_bullet", "resources/textures/projectiles/player_bullet.png");
    loadTexture("enemy_bullet", "resources/textures/projectiles/enemy_bullet.png");
    loadTexture("heavy_bullet", "resources/textures/projectiles/heavy_bullet.png");
    
    // Try to load main menu background
    loadTexture("main_menu_background", "resources/textures/backgrounds/main_menu_bg.png");
    
    // Create procedural background if no background file found
    createBackgroundTexture();
    
    std::cout << "Texture loading completed. Loaded " << m_textures.size() << " textures." << std::endl;
}

void ResourceManager::createBackgroundTexture() {
    // Create a beautiful space background procedurally
    const unsigned int width = 1280;
    const unsigned int height = 960;
    
    sf::RenderTexture renderTexture;
    if (!renderTexture.resize({width, height})) {
        std::cerr << "Failed to create background render texture" << std::endl;
        return;
    }
    
    // Dark space background with nebula-like effect
    sf::RectangleShape background(sf::Vector2f(static_cast<float>(width), static_cast<float>(height)));
    background.setFillColor(sf::Color(5, 10, 25)); // Dark blue space
    renderTexture.draw(background);
    
    // Create stars
    std::mt19937 rng(42); // Fixed seed for consistent background
    std::uniform_real_distribution<float> xDist(0, width);
    std::uniform_real_distribution<float> yDist(0, height);
    std::uniform_real_distribution<float> sizeDist(0.5f, 2.5f);
    std::uniform_int_distribution<int> alphaDist(50, 255);
    
    for (int i = 0; i < 300; ++i) {
        sf::CircleShape star(sizeDist(rng));
        star.setPosition({xDist(rng), yDist(rng)});
        std::uint8_t alpha = static_cast<std::uint8_t>(alphaDist(rng));
        star.setFillColor(sf::Color(255, 255, 255, alpha));
        renderTexture.draw(star);
    }
    
    // Add some colored nebula effects
    for (int i = 0; i < 8; ++i) {
        sf::CircleShape nebula(sizeDist(rng) * 30 + 40);
        nebula.setPosition({xDist(rng), yDist(rng)});
        
        // Random nebula colors
        sf::Color nebulaColors[] = {
            sf::Color(80, 30, 120, 30),   // Purple
            sf::Color(120, 60, 30, 30),   // Orange  
            sf::Color(30, 80, 120, 30),   // Blue
            sf::Color(120, 30, 60, 30)    // Magenta
        };
        
        nebula.setFillColor(nebulaColors[i % 4]);
        renderTexture.draw(nebula);
    }
    
    renderTexture.display();
    m_textures["space_background"] = renderTexture.getTexture();
    std::cout << "Created procedural space background texture" << std::endl;
}
