#ifndef SFUI_EDIT_H_INCLUDED
#define SFUI_EDIT_H_INCLUDED

#include "details/ComponentBase.h"

namespace sfui {

class Edit: public ComponentBase {
public:
    Edit(ComponentBase& parent, const nlohmann::json& json);
    ~Edit() = default;

    void SetBackgroundColor(sf::Color color);
    void SetBackgroundColor(std::uint32_t color) {
        SetBackgroundColor(sf::Color(color));
    }

    void Render(sf::RenderWindow& window) override;

private:
    void Render_(sf::RenderWindow& window, sf::RectangleShape& rectangle);
    void Render_(sf::RenderWindow& window, sf::Text& text);

private:
    std::optional<sf::RectangleShape> m_background;
    sf::Text m_text;
    bool m_focused;
    bool m_showCursor;
    sf::Clock m_cursorBlinkTimer;
};

}

#endif // SFUI_EDIT_H_INCLUDED
