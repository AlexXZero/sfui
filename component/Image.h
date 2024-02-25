#ifndef SFUI_IMAGE_H_INCLUDED
#define SFUI_IMAGE_H_INCLUDED

#include "details/Base.h"

namespace sfui {

class Image: public ComponentBase {
public:
    Image(ComponentContainer& parent, const nlohmann::json& json) : ComponentBase(parent, json) {
        // parse optional properties
        if (json.contains("image")) {
            m_texture.loadFromFile(json["image"].get<std::string>());
            m_rectangle.setTexture(&m_texture);
            auto [width, height] = m_texture.getSize();
            if (!json.contains("width")) SetWidth(width);
            if (!json.contains("height")) SetHeight(height);
            m_rectangle.setSize({Width(), Height()});
            m_rectangle.setPosition(AbsoluteX(), AbsoluteY());
            //OnResize([this](std::uint16_t width, std::uint16_t height){ m_rectangle.setSize(sf::Vector2f(width, height)); });
            //OnMove([this](std::int16_t x, std::int16_t y){ m_rectangle.setPosition(x, y); });
        }
    }
    ~Image() = default;

protected:
    void Render(sf::RenderWindow& window) override {
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

private:
    sf::Texture m_texture;
    sf::RectangleShape m_rectangle;
};

}

#endif // SFUI_IMAGE_H_INCLUDED
