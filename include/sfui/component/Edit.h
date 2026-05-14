#ifndef SFUI_EDIT_H_INCLUDED
#define SFUI_EDIT_H_INCLUDED

#include "details/ComponentBase.h"

namespace sfui {

class Edit: public ComponentBase {
public:
    static constexpr std::string_view ComponentTypeName = "edit";
public:
    Edit(ComponentBase& parent, ConfigView config);
    ~Edit() = default;

    void SetBackgroundColor(sf::Color color);
    void SetBackgroundColor(std::uint32_t color) {
        SetBackgroundColor(sf::Color(color));
    }

    void Render(sf::RenderWindow& window) override;

private:
    void Render_(sf::RenderWindow& window, sf::RectangleShape& rectangle);
    void Render_(sf::RenderWindow& window, sf::Text& text);
    void TextEnteredHandler(uint32_t unicode);

private:
    std::optional<sf::RectangleShape> m_background;
    sf::Text m_text;
    bool m_focused;
    bool m_showCursor;
    sf::Clock m_cursorBlinkTimer;
};

}

#endif // SFUI_EDIT_H_INCLUDED
