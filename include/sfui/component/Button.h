#ifndef SFUI_BUTTON_H_INCLUDED
#define SFUI_BUTTON_H_INCLUDED

#include "details/ComponentBase.h"
#include "Image.h"
#include "Label.h"

namespace sfui {

class Button: public ComponentBase {
    enum State { Neutral, Active, Down, Count };
public:
    static constexpr std::string_view ComponentTypeName = "button";

public:
    Button(ComponentBase& parent, ConfigView config) :
            ComponentBase(parent, config), m_images{
                Image(*this, GetImageProperties(config, "neutral")),
                Image(*this, GetImageProperties(config, "active")),
                Image(*this, GetImageProperties(config, "down"))} {
        // parse optional properties
        if (config.contains("image") && (!config.contains("width") || !config.contains("height"))) {
#if 0
            if (config["image"].is_string()) {
                auto [width, height] = m_images[State::Neutral].GetNativeSize();
                if (!config.contains("width")) SetWidth(width);
                if (!config.contains("height")) SetHeight(height);
                //OnResize([this](std::uint16_t width, std::uint16_t height){ m_image[State::Neutral].setSize(sf::Vector2f(width, height)); });
                //OnMove([this](std::int16_t x, std::int16_t y){ m_image[State::Neutral].setPosition(x, y); });
            } else {
            }
#endif
            auto imageConfig = config.required("image");
            if (imageConfig.contains("down")) {
                auto [width, height] = m_images[State::Down].GetNativeSize();
                if (!config.contains("width")) SetWidth(width);
                if (!config.contains("height")) SetHeight(height);
            }
            if (imageConfig.contains("active")) {
                auto [width, height] = m_images[State::Active].GetNativeSize();
                if (!config.contains("width")) SetWidth(width);
                if (!config.contains("height")) SetHeight(height);
            }
            if (imageConfig.contains("neutral")) {
                auto [width, height] = m_images[State::Neutral].GetNativeSize();
                if (!config.contains("width")) SetWidth(width);
                if (!config.contains("height")) SetHeight(height);
            }
        }
        if (config.contains("label")) m_label.emplace(*this, GetLabelProperties(config));
        LinkEvent(OnMouseEnter([this]{ m_state = Active; }));
        LinkEvent(OnMouseLeave([this]{ m_state = Neutral; }));
        LinkEvent(OnMouseButtonPress([this](){ m_state = Down; }));
        LinkEvent(OnMouseButtonRelease([this](){ m_state = Active; }));
        LinkEvent(OnResize([this]{
            for (auto& i: m_images) i.SetSize(Width(), Height());
            if (m_label.has_value()) m_label->SetSize(Width(), Height());
        }));
        LinkEvent(OnMove([this]{
            for (auto& i: m_images) i.SetPosition(0, 0);
            if (m_label.has_value()) m_label->SetPosition(0, 0);
        }));
    }
    ~Button() = default;

    void Render(sf::RenderWindow& window) override {
        m_images[m_state].Render(window);
        if (m_label.has_value()) m_label.value().Render(window);
    }

private:
    static Image::Properties GetImageProperties(ConfigView config, const std::string& state) {
        nlohmann::json properties = {{"name", std::string("_") + state}};
        if (auto imageConfig = config.optional("image")) {
            if (auto imageState = imageConfig->optional(state)) {
                properties += {"image", imageState->raw()};
            }
        }
        return Image::Properties(ConfigView(properties));
    }
    static Label::Properties GetLabelProperties(ConfigView config) {
        nlohmann::json properties = {{"name", "_label"}, {"text-alignment", "center"}};
        if (auto label = config.optional("label")) properties += {"text", label->raw()};
        if (auto font = config.optional("font")) properties += {"font", font->raw()};
        if (auto fontSize = config.optional("font-size")) properties += {"height", fontSize->raw()};
        if (auto textColor = config.optional("text-color")) properties += {"text-color", textColor->raw()};
        return Label::Properties(ConfigView(properties));
    }

private:
    std::array<Image, State::Count> m_images;
    std::optional<Label> m_label;
    State m_state = State::Neutral;
};

}

#endif // SFUI_BUTTON_H_INCLUDED
