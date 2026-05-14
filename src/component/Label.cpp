#include "component/Label.h"
#include "component/details/Parsers.h"
#include "FontLibrary.h"
#include <CxxUtils/unicode.h>
#include <SFML/Graphics/Font.hpp>

using namespace sfui;

Label::Properties::Properties(ConfigView config)
    : ComponentBase::Properties(config)
    , backgroundColor{config.optional<sf::Color>("background-color")}
    , font{std::nullopt}
    , text{config.optional<std::string>("text")}
    , textColor{config.optional<sf::Color>("text-color")}
    , textStyle{sf::Text::Regular}
    , textAlignment{TextAlignment::Left}
{
    if (auto fontPath = config.optional<std::string>("font")) font = FontLibrary::Get(*fontPath);
    if (config.valueOr<bool>("bold", false)) textStyle |= sf::Text::Bold;
    if (config.valueOr<bool>("italic", false)) textStyle |= sf::Text::Italic;
    if (config.valueOr<bool>("underlined", false)) textStyle |= sf::Text::Underlined;
    if (config.valueOr<bool>("strike-through", false)) textStyle |= sf::Text::StrikeThrough;
    if (auto alignment = config.optional<std::string>("text-alignment")) {
        textAlignment =
            *alignment == "left" ? TextAlignment::Left :
            *alignment == "right" ? TextAlignment::Right :
            *alignment == "center" ? TextAlignment::Center :
            throw std::runtime_error("unknown alignment value: " + *alignment);
    }
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
        m_text.setString(CxxUtils::s2ws(properties.text.value()));
    }

    if (properties.textColor.has_value()) {
        m_text.setFillColor(properties.textColor.value());
    }

    m_text.setStyle(properties.textStyle);
    m_textAlignment = properties.textAlignment;

    m_text.setCharacterSize(Height() - 4); // in pixels
    m_text.setPosition(GetTextRenderPosition(m_text, m_textAlignment));

    LinkEvent(OnResize([this]{
        m_text.setCharacterSize(Height() - 4);
        m_text.setPosition(GetTextRenderPosition(m_text, m_textAlignment));
        if (m_background.has_value()) m_background->setSize(sf::Vector2f(Width(), Height()));
    }));
    LinkEvent(OnMove([this]{
        m_text.setCharacterSize(Height() - 4);
        m_text.setPosition(GetTextRenderPosition(m_text, m_textAlignment));
        if (m_background.has_value()) m_background->setPosition(AbsoluteX(), AbsoluteY());
    }));
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
        window.draw(m_background.value());
    }

    window.draw(m_text);
}

sf::Vector2f Label::GetTextRenderPosition(sf::Text& text, TextAlignment alignment)
{
    const sf::FloatRect textLocalBounds = text.getLocalBounds();
    return alignment == TextAlignment::Left ? sf::Vector2f(AbsoluteX(), AbsoluteY())
         : alignment == TextAlignment::Right ? sf::Vector2f(AbsoluteX() + Width() - textLocalBounds.width, AbsoluteY())
         : alignment == TextAlignment::Center ? sf::Vector2f(AbsoluteX() + (Width() - textLocalBounds.width) / 2, AbsoluteY())
         : throw std::runtime_error("unknown alignment value: " + alignment);
}
