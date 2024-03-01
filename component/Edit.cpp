#include "Edit.h"
#include "details/Parsers.h"
#include "../FontLibrary.h"
#include <SFML/Graphics/Font.hpp>

namespace sfui {

Edit::Edit(ComponentBase& parent, const nlohmann::json& json) : ComponentBase(parent, json)
{
    // parse optional properties
    if (json.contains("background-color")) {
        SetBackgroundColor(ParseColor(json["background-color"]));
    }

    if (json.contains("font")) {
        m_text.setFont(FontLibrary::Get(json["font"]));
    } else {
        m_text.setFont(FontLibrary::GetDefaultFont());
    }

    if (json.contains("text")) {
        m_text.setString(json["text"].get<std::string>());
    }

    if (json.contains("text-color")) {
        m_text.setFillColor(ParseColor(json["text-color"]));
    }

    //m_text.setStyle(sf::Text::Bold | sf::Text::Underlined); TODO: json["text-style"] or json["bold"]

    m_text.setCharacterSize(Height() - 4); // in pixels
    m_text.setPosition(AbsoluteX(), AbsoluteY());

    m_focused = false;
    LinkEvent(OnGainedFocus([this](){ m_focused = true; m_showCursor = true; m_cursorBlinkTimer.restart(); }));
    LinkEvent(OnLostFocus([this](){ m_focused = false; }));
    // TODO: LinkEvent(OnTextEnter([this](std::uint32_t unicode){ m_text += unicode; }));
}

void Edit::SetBackgroundColor(sf::Color color)
{
    m_background.emplace(sf::Vector2f(Width(), Height()));
    m_background->setFillColor(sf::Color(color));
    m_background->setPosition(AbsoluteX(), AbsoluteY());
}

void Edit::Render(sf::RenderWindow& window)
{
    if (m_background.has_value()) {
        Render_(window, m_background.value());
    }

    Render_(window, m_text);

    if (m_focused) { // Render cursor
        if (m_cursorBlinkTimer.getElapsedTime().asMilliseconds () > 500) {
            m_cursorBlinkTimer.restart();
            m_showCursor = !m_showCursor;
        }
        if (m_showCursor) {
            sf::Vector2f cursorPosition(m_text.getPosition().x + m_text.getGlobalBounds().width + 5, m_text.getPosition().y + 2);
            sf::Vertex line[] = {
                sf::Vertex(cursorPosition, sf::Color::Black),
                sf::Vertex(sf::Vector2f(cursorPosition.x, cursorPosition.y + m_text.getCharacterSize()), sf::Color::Black)
            };
            window.draw(line, 2, sf::Lines);
        }
    }
}

void Edit::Render_(sf::RenderWindow& window, sf::RectangleShape& rectangle)
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

void Edit::Render_(sf::RenderWindow& window, sf::Text& text)
{
    const sf::Vector2f old_position = text.getPosition();
    const sf::Vector2f new_position(AbsoluteX(), AbsoluteY());
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

}
