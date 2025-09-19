#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <unordered_map>
#include <string>

class ResourceManager {
private:
    sf::Font m_font;
    bool m_fontLoaded = false;
    
    // Texture management
    std::unordered_map<std::string, sf::Texture> m_textures;
    
    // Helper methods
    std::string findProjectRoot() const;
    
public:
    bool loadSystemFont();
    
    [[nodiscard]] bool isFontLoaded() const noexcept { return m_fontLoaded; }
    [[nodiscard]] const sf::Font& getFont() const noexcept { return m_font; }
    
    // Texture methods
    bool loadTexture(const std::string& name, const std::string& filePath);
    [[nodiscard]] bool hasTexture(const std::string& name) const noexcept;
    [[nodiscard]] const sf::Texture& getTexture(const std::string& name) const;
    void loadAllGameTextures();
    void createBackgroundTexture(); // Create procedural space background
    
    // Sprite scaling utility
    void scaleTextureToExactSize(sf::Sprite& sprite, float targetWidth, float targetHeight) const;
};
