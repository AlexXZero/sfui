#ifndef SFUI_LABEL_H_INCLUDED
#define SFUI_LABEL_H_INCLUDED

#include "details/Base.h"

namespace sfui {

class Label: public ComponentBase {
public:
    Label(Component& parent, const nlohmann::json& json);
    ~Label() = default;

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
};

}

#endif // SFUI_LABEL_H_INCLUDED
