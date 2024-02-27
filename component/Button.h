#ifndef SFUI_BUTTON_H_INCLUDED
#define SFUI_BUTTON_H_INCLUDED

#include "details/Base.h"
#include <iostream>

namespace sfui {

class Button: public ComponentBase {
    enum State { Neutral, Active, Down, Count };

public:
    Button(ComponentContainer& parent, const nlohmann::json& json) : ComponentBase(parent, json) {
        // parse optional properties
        if (json.contains("image")) {
#if 0
            if (json["image"].is_string()) {
                m_texture.loadFromFile(json["image"].get<std::string>());
                m_rectangle.setTexture(&m_texture);
                auto [width, height] = m_texture.getSize();
                if (!json.contains("width")) SetWidth(width);
                if (!json.contains("height")) SetHeight(height);
                m_rectangle.setSize({Width(), Height()});
                m_rectangle.setPosition(AbsoluteX(), AbsoluteY());
                //OnResize([this](std::uint16_t width, std::uint16_t height){ m_rectangle.setSize(sf::Vector2f(width, height)); });
                //OnMove([this](std::int16_t x, std::int16_t y){ m_rectangle.setPosition(x, y); });
            } else {
            }
#endif
            if (json["image"].contains("down")) {
                sf::Texture texture;
                texture.loadFromFile(json["image"]["down"].get<std::string>());
                auto [width, height] = texture.getSize();
                if (!json.contains("width")) SetWidth(width);
                if (!json.contains("height")) SetHeight(height);
                m_rectangle.setSize({Width(), Height()});
                m_rectangle.setPosition(AbsoluteX(), AbsoluteY());
                m_textures[State::Down] = texture;
                m_rectangle.setTexture(&m_textures[State::Down].value());
            }
            if (json["image"].contains("active")) {
                sf::Texture texture;
                texture.loadFromFile(json["image"]["active"].get<std::string>());
                auto [width, height] = texture.getSize();
                if (!json.contains("width")) SetWidth(width);
                if (!json.contains("height")) SetHeight(height);
                m_rectangle.setSize({Width(), Height()});
                m_rectangle.setPosition(AbsoluteX(), AbsoluteY());
                m_textures[State::Active] = texture;
                m_rectangle.setTexture(&m_textures[State::Active].value());
            }
            if (json["image"].contains("neutral")) {
                sf::Texture texture;
                texture.loadFromFile(json["image"]["neutral"].get<std::string>());
                auto [width, height] = texture.getSize();
                if (!json.contains("width")) SetWidth(width);
                if (!json.contains("height")) SetHeight(height);
                m_rectangle.setSize({Width(), Height()});
                m_rectangle.setPosition(AbsoluteX(), AbsoluteY());
                m_textures[State::Neutral] = texture;
                m_rectangle.setTexture(&m_textures[State::Neutral].value());
            }
        }
        LinkEvent(OnMouseEnter([this]{ m_state = Active; m_rectangle.setTexture(&m_textures[m_state].value()); }));
        LinkEvent(OnMouseLeave([this]{ m_state = Neutral; m_rectangle.setTexture(&m_textures[m_state].value()); }));
    }
    ~Button() = default;

    void Render(sf::RenderWindow& window) override {
        if (m_textures[m_state].has_value()) {
            const sf::Vector2f old_position = m_rectangle.getPosition();
            const sf::Vector2f new_position(AbsoluteX(), AbsoluteY());
            if (old_position != new_position) {
                m_rectangle.setPosition(new_position);
            }

            const sf::Vector2f old_size = m_rectangle.getSize();
            const sf::Vector2f new_size(Width(), Height());
            if (old_size != new_size) {
                m_rectangle.setSize(new_size);
            }

    #if 0 // TODO (there is a wrong version, more complex path should be done here and for position as well)
            const sf::IntRect old_texture_rect = m_rectangle.getTextureRect();
            const sf::IntRect new_texture_rect(Left() < 0 Left() : 0, Parent().AbsoluteY(), Parent().Width(), Parent().Height());
            if (old_texture_rect != new_texture_rect) {
                m_rectangle.setTextureRect(new_texture_rect);
            }
    #endif

            window.draw(m_rectangle);
        }
    }

private:
    std::array<std::optional<sf::Texture>, State::Count> m_textures;
    sf::RectangleShape m_rectangle;
    State m_state = State::Neutral;
};

}

#endif // SFUI_BUTTON_H_INCLUDED
