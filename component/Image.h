#ifndef SFUI_IMAGE_H_INCLUDED
#define SFUI_IMAGE_H_INCLUDED

#include "details/Base.h"

namespace sfui {

class Image: public ComponentBase {
public:
    Image(Component& parent, const nlohmann::json& json) : ComponentBase(parent, json) {
        // parse optional properties
        if (json.contains("image")) {
            SetImage(json["image"].get<std::string>());
            auto [width, height] = m_texture.getSize();
            if (!json.contains("width")) SetWidth(width);
            if (!json.contains("height")) SetHeight(height);
            m_image->setSize({float(Width()), float(Height())});
            m_image->setPosition(AbsoluteX(), AbsoluteY());
            //OnResize([this](std::uint16_t width, std::uint16_t height){ m_image->setSize(sf::Vector2f(width, height)); });
            //OnMove([this](std::int16_t x, std::int16_t y){ m_image->setPosition(x, y); });
        }

        if (json.contains("background-color")) {
            SetBackgroundColor(ParseColor(json["background-color"]));
        }
    }
    ~Image() = default;

    void SetBackgroundColor(sf::Color color) {
        m_background.emplace(sf::Vector2f(Width(), Height()));
        m_background->setFillColor(sf::Color(color));
        m_background->setPosition(AbsoluteX(), AbsoluteY());
    }

    void SetBackgroundColor(std::uint32_t color) {
        SetBackgroundColor(sf::Color(color));
    }

    void SetImage(const std::string& image_path) {
        m_image.emplace();
        m_texture.loadFromFile(image_path);
        m_image->setTexture(&m_texture);
    }

    std::pair<uint16_t, uint16_t> GetNativeSize() const {
        auto [width, height] = m_texture.getSize();
        return {width, height};
    }

    void Render(sf::RenderWindow& window) override {
        if (m_background.has_value()) {
            Render_(window, m_background.value());
        }
        if (m_image.has_value()) {
            Render_(window, m_image.value());
        }
    }

private:
    void Render_(sf::RenderWindow& window, sf::RectangleShape& rectangle) {
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

private:
    sf::Texture m_texture;
    std::optional<sf::RectangleShape> m_image;
    std::optional<sf::RectangleShape> m_background;
};

}

#endif // SFUI_IMAGE_H_INCLUDED
