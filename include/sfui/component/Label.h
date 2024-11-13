#ifndef SFUI_LABEL_H_INCLUDED
#define SFUI_LABEL_H_INCLUDED

#include "details/ComponentBase.h"

namespace sfui {

class Label: public ComponentBase {
public:
    enum TextAlignment { Left, Right, Center };

    struct Properties: public ComponentBase::Properties {
        std::optional<sf::Color> backgroundColor;
        std::optional<sf::Font> font;
        std::optional<std::string> text;
        std::optional<sf::Color> textColor;
        std::uint32_t textStyle = sf::Text::Regular;
        TextAlignment textAlignment = TextAlignment::Left;

        Properties() = default;
        Properties(const nlohmann::json& json);
    };

    Label(ComponentBase& parent, const Properties& properties);
    Label(ComponentBase& parent, const nlohmann::json& json)
        : Label(parent, Properties(json)) { ParseHandlers(json); }
    ~Label() = default;

    void SetBackgroundColor(sf::Color color);
    void SetBackgroundColor(std::uint32_t color) {
        SetBackgroundColor(sf::Color(color));
    }

    void Render(sf::RenderWindow& window) override;

private:
    sf::Vector2f GetTextRenderPosition(sf::Text& text, TextAlignment alignment);

private:
    std::optional<sf::RectangleShape> m_background;
    TextAlignment m_textAlignment;
    sf::Text m_text;
};

}

#endif // SFUI_LABEL_H_INCLUDED
