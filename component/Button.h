#ifndef SFUI_BUTTON_H_INCLUDED
#define SFUI_BUTTON_H_INCLUDED

#include "details/ComponentBase.h"
#include "Image.h"

namespace sfui {

class Button: public ComponentBase {
    enum State { Neutral, Active, Down, Count };

public:
    Button(ComponentBase& parent, const nlohmann::json& json) :
            ComponentBase(parent, json), m_images{
                Image(*this, GetImageProperties(json, "neutral")),
                Image(*this, GetImageProperties(json, "active")),
                Image(*this, GetImageProperties(json, "down"))} {
        // parse optional properties
        if (json.contains("image") && (!json.contains("width") || !json.contains("height"))) {
#if 0
            if (json["image"].is_string()) {
                auto [width, height] = m_images[State::Neutral].GetNativeSize();
                if (!json.contains("width")) SetWidth(width);
                if (!json.contains("height")) SetHeight(height);
                //OnResize([this](std::uint16_t width, std::uint16_t height){ m_image[State::Neutral].setSize(sf::Vector2f(width, height)); });
                //OnMove([this](std::int16_t x, std::int16_t y){ m_image[State::Neutral].setPosition(x, y); });
            } else {
            }
#endif
            if (json["image"].contains("down")) {
                auto [width, height] = m_images[State::Down].GetNativeSize();
                if (!json.contains("width")) SetWidth(width);
                if (!json.contains("height")) SetHeight(height);
            }
            if (json["image"].contains("active")) {
                auto [width, height] = m_images[State::Active].GetNativeSize();
                if (!json.contains("width")) SetWidth(width);
                if (!json.contains("height")) SetHeight(height);
            }
            if (json["image"].contains("neutral")) {
                auto [width, height] = m_images[State::Neutral].GetNativeSize();
                if (!json.contains("width")) SetWidth(width);
                if (!json.contains("height")) SetHeight(height);
            }
        }
        LinkEvent(OnMouseEnter([this]{ m_state = Active; }));
        LinkEvent(OnMouseLeave([this]{ m_state = Neutral; }));
    }
    ~Button() = default;

    void Render(sf::RenderWindow& window) override {
        m_images[m_state].Render(window);
    }

private:
    static nlohmann::json GetImageProperties(nlohmann::json json, const std::string& state) {
        nlohmann::json properties {{"name", std::string("_") + state}};
        if (json.contains("image") && json["image"].contains(state)) properties += {"image", json["image"][state]};
        return properties;
    }

private:
    std::array<Image, State::Count> m_images;
    State m_state = State::Neutral;
};

}

#endif // SFUI_BUTTON_H_INCLUDED
