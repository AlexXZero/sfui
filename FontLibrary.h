#ifndef SFUI_FONT_LIBRARY_H_INCLUDED
#define SFUI_FONT_LIBRARY_H_INCLUDED

#include <string>

namespace sf {
class Font;
};

namespace sfui {

class FontLibrary {
public:
    static void Load(const std::string& fontName, const std::string& fontPath);
    static const sf::Font& Get(const std::string& fontName);

    static void SetDefaultFont(const sf::Font& font);
    static const sf::Font& GetDefaultFont();
};

}

#endif // SFUI_FONT_LIBRARY_H_INCLUDED
