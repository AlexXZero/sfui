#ifndef SFUI_LABEL_H_INCLUDED
#define SFUI_LABEL_H_INCLUDED

#include "details/ComponentBase.h"
#include "details/Parsers.h"

namespace sfui {

class Label: public ComponentBase {
public:
    static constexpr std::string_view ComponentTypeName = "label";
public:
    struct Properties: public ComponentBase::Properties {
        std::optional<sf::Color> backgroundColor;
        std::optional<std::string> fontName;
        std::optional<std::string> text;
        std::optional<sf::Color> textColor;
        std::optional<TextStyle> textStyle;
        std::optional<TextAlignment> textAlignment;

        Properties() = default;
        Properties(ConfigView config, const Properties& defaults = {});
    };

    Label(ComponentBase& parent, const Properties& properties);
    Label(ComponentBase& parent, const ConfigView config)
        : Label(parent, Properties(config)) { ParseHandlers(config); }
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
