#include "component/Label.h"
#include "component/details/Parsers.h"
#include "FontLibrary.h"
#include <CxxUtils/unicode.h>
#include <SFML/Graphics/Font.hpp>

using namespace sfui;

using sfui::optional_fallback::operator||;
Label::Properties::Properties(ConfigView config, const Properties& defaults)
    : ComponentBase::Properties(config, defaults)
    , backgroundColor   {config.optional<sf::Color>("background-color")   or defaults.backgroundColor}
    , fontName          {config.optional<std::string>("font")             or defaults.fontName}
    , text              {config.optional<std::string>("text")             or defaults.text}
    , textColor         {config.optional<sf::Color>("text-color")         or defaults.textColor}
    , textStyle         {config.optional<TextStyle>("text-style")         or defaults.textStyle}
    , textAlignment     {config.optional<TextAlignment>("text-alignment") or defaults.textAlignment}
{}

Label::Label(ComponentBase& parent, const Properties& properties) : ComponentBase(parent, properties)
{
    if (properties.backgroundColor.has_value()) {
        SetBackgroundColor(properties.backgroundColor.value());
    }

    if (properties.fontName.has_value()) {
        m_text.setFont(FontLibrary::Get(properties.fontName.value()));
    } else {
        m_text.setFont(FontLibrary::GetDefaultFont());
    }

    if (properties.text.has_value()) {
        m_text.setString(CxxUtils::s2ws(properties.text.value()));
    }

    if (properties.textColor.has_value()) {
        m_text.setFillColor(properties.textColor.value());
    }

    m_text.setStyle(properties.textStyle.value_or(sf::Text::Regular));
    m_textAlignment = properties.textAlignment.value_or(TextAlignment::Left);

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

void Label::Render(sf::RenderTarget& surface)
{
    if (m_background.has_value()) {
        surface.draw(m_background.value());
    }

    surface.draw(m_text);
}

sf::Vector2f Label::GetTextRenderPosition(sf::Text& text, TextAlignment alignment)
{
    const sf::FloatRect textLocalBounds = text.getLocalBounds();
    return alignment == TextAlignment::Left ? sf::Vector2f(AbsoluteX(), AbsoluteY())
         : alignment == TextAlignment::Right ? sf::Vector2f(AbsoluteX() + Width() - textLocalBounds.width, AbsoluteY())
         : alignment == TextAlignment::Center ? sf::Vector2f(AbsoluteX() + (Width() - textLocalBounds.width) / 2, AbsoluteY())
         : throw std::runtime_error("unknown alignment value: " + alignment);
}
