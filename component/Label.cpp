#include "Label.h"
#include "details/Parsers.h"
#include "../FontLibrary.h"
#include "../utils/unicode.h"
#include <SFML/Graphics/Font.hpp>

using namespace sfui;

Label::Properties::Properties(const nlohmann::json& json) : ComponentBase::Properties(json)
{
    if (json.contains("background-color")) backgroundColor = ParseColor(json["background-color"]);
    if (json.contains("font")) font = FontLibrary::Get(json["font"]);
    if (json.contains("text")) text = json["text"].get<std::string>();
    if (json.contains("text-color")) textColor = ParseColor(json["text-color"]);
    if (json.contains("bold") && json["bold"].get<bool>()) textStyle |= sf::Text::Bold;
    if (json.contains("italic") && json["italic"].get<bool>()) textStyle |= sf::Text::Italic;
    if (json.contains("underlined") && json["underlined"].get<bool>()) textStyle |= sf::Text::Underlined;
    if (json.contains("strike-through") && json["strike-through"].get<bool>()) textStyle |= sf::Text::StrikeThrough;
    if (json.contains("text-alignment")) textAlignment =
            json["text-alignment"] == "left" ? TextAlignment::Left :
            json["text-alignment"] == "right" ? TextAlignment::Right :
            json["text-alignment"] == "center" ? TextAlignment::Center :
            throw std::runtime_error("unknown alignment value: " + json["text-alignment"].get<std::string>());
}

Label::Label(ComponentBase& parent, const Properties& properties) : ComponentBase(parent, properties)
{
    if (properties.backgroundColor.has_value()) {
        SetBackgroundColor(properties.backgroundColor.value());
    }

    if (properties.font.has_value()) {
        m_text.setFont(properties.font.value());
    } else {
        m_text.setFont(FontLibrary::GetDefaultFont());
    }

    if (properties.text.has_value()) {
        m_text.setString(utils::s2ws(properties.text.value()));
    }

    if (properties.textColor.has_value()) {
        m_text.setFillColor(properties.textColor.value());
    }

    m_text.setStyle(properties.textStyle);
    m_textAlignment = properties.textAlignment;

    m_text.setCharacterSize(Height() - 4); // in pixels
    m_text.setPosition(AbsoluteX(), AbsoluteY());
}

void Label::SetBackgroundColor(sf::Color color)
{
    m_background.emplace(sf::Vector2f(Width(), Height()));
    m_background->setFillColor(sf::Color(color));
    m_background->setPosition(AbsoluteX(), AbsoluteY());
}

void Label::Render(sf::RenderWindow& window)
{
    if (m_background.has_value()) {
        Render_(window, m_background.value());
    }

    Render_(window, m_text, m_textAlignment);
}

void Label::Render_(sf::RenderWindow& window, sf::RectangleShape& rectangle)
{
    const sf::Vector2f old_position = rectangle.getPosition();
    const sf::Vector2f new_position(AbsoluteX(), AbsoluteY());
    if (old_position != new_position) {
        rectangle.setPosition(new_position);
    }

    const sf::Vector2f old_size = rectangle.getSize();
    const sf::Vector2f new_size(Width(), Height());
    if (old_size != new_size) {
        rectangle.setSize(new_size);
    }

#if 0 // TODO (there is a wrong version, more complex path should be done here and for position as well)
    const sf::IntRect old_texture_rect = rectangle.getTextureRect();
    const sf::IntRect new_texture_rect(Left() < 0 Left() : 0, Parent().AbsoluteY(), Parent().Width(), Parent().Height());
    if (old_texture_rect != new_texture_rect) {
        rectangle.setTextureRect(new_texture_rect);
    }
#endif

    window.draw(rectangle);
}

void Label::Render_(sf::RenderWindow& window, sf::Text& text, TextAlignment alignment)
{
    const sf::Vector2f old_position = text.getPosition();
    const sf::FloatRect textLocalBounds = text.getLocalBounds();
    const sf::Vector2f new_position =
        alignment == TextAlignment::Left ? sf::Vector2f(AbsoluteX(), AbsoluteY()) :
        alignment == TextAlignment::Right ? sf::Vector2f(AbsoluteX() + Width() - textLocalBounds.width, AbsoluteY()) :
        alignment == TextAlignment::Center ? sf::Vector2f(AbsoluteX() + (Width() - textLocalBounds.width) / 2, AbsoluteY()) :
        throw std::runtime_error("unknown alignment value: " + alignment);
    if (old_position != new_position) {
        text.setPosition(new_position);
    }

    const unsigned int old_size = text.getCharacterSize();
    const unsigned int new_size = Height() - 4;
    if (old_size != new_size) {
        text.setCharacterSize(new_size);
    }

    window.draw(text);
}
