#ifndef SFUI_FORM_H_INCLUDED
#define SFUI_FORM_H_INCLUDED

#include "details/ComponentBase.h"
#include "details/Parsers.h"

namespace sfui {

class Form: public ComponentBase {
public:
    Form(ComponentBase& parent, const nlohmann::json& json) : ComponentBase(parent, json) {
        // parse optional properties
        if (json.contains("background")) SetBackground(ParseColor(json["background"]));
        if (json.contains("background-image")) {
            SetBackground(json["background-image"].get<std::string>());
            auto [width, height] = m_texture.getSize();
            if (!json.contains("width")) SetWidth(width);
            if (!json.contains("height")) SetHeight(height);
        }
    }
    ~Form() = default;

    void SetBackground(sf::Color color) {
        InitialiseBackground();
        m_background->setFillColor(sf::Color(color));
    }

    void SetBackground(std::uint32_t color) {
        SetBackground(sf::Color(color));
    }

    void SetBackground(const std::string& image_path) {
        InitialiseBackground();
        m_texture.loadFromFile(image_path);
        m_background->setTexture(&m_texture);
    }

    void Render(sf::RenderWindow& window) override {
        if (m_background.has_value()) {
            const sf::Vector2f old_position = m_background->getPosition();
            const sf::Vector2f new_position(AbsoluteX(), AbsoluteY());
            if (old_position != new_position) {
                m_background->setPosition(new_position);
            }

            const sf::Vector2f old_size = m_background->getSize();
            const sf::Vector2f new_size(Width(), Height());
            if (old_size != new_size) {
                m_background->setSize(new_size);
            }

#if 0 // TODO (there is a wrong version, more complex path should be done here and for position as well)
            const sf::IntRect old_texture_rect = m_background->getTextureRect();
            const sf::IntRect new_texture_rect(Left() < 0 Left() : 0, Parent().AbsoluteY(), Parent().Width(), Parent().Height());
            if (old_texture_rect != new_texture_rect) {
                m_background->setTextureRect(new_texture_rect);
            }
#endif

            window.draw(m_background.value());
        }
    }

private:
    void InitialiseBackground() {
        if (!m_background.has_value()) {
            m_background = sf::RectangleShape(/*sf::Vector2f(Width(), Height())*/);
            m_background->setPosition(AbsoluteX(), AbsoluteY());
            //OnResize([this](std::uint16_t width, std::uint16_t height){ m_background->setSize(sf::Vector2f(width, height)); });
            //OnMove([this](std::int16_t x, std::int16_t y){ m_background->setPosition(x, y); });
        }
    }

private:
    std::optional<sf::RectangleShape> m_background;
    sf::Texture m_texture;
};

}

#endif // SFUI_FORM_H_INCLUDED
