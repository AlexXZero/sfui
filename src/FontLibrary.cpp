#include "FontLibrary.h"
#include <optional>
#include <unordered_map>
#include <SFML/Graphics/Font.hpp>

namespace sfui {

static std::unordered_map<std::string, sf::Font> fonts;
static std::optional<sf::Font> defaultFont;

void FontLibrary::Load(const std::string& fontName, const std::string& fontPath)
{
    sf::Font font;
    if (!font.loadFromFile(fontPath)) {
        throw std::runtime_error("Error loading font \"" + fontName + "\": " + fontPath);
    }
    fonts[fontName] = font;
}

const sf::Font& FontLibrary::Get(const std::string& fontName)
{
    return fonts[fontName];
}

void FontLibrary::SetDefaultFont(const sf::Font& font)
{
    defaultFont.emplace(font);
}

const sf::Font& FontLibrary::GetDefaultFont()
{
    return defaultFont.value();
}

}
