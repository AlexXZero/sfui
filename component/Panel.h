#ifndef SFUI_PANEL_H_INCLUDED
#define SFUI_PANEL_H_INCLUDED

#include "details/ComponentBase.h"
#include "details/Parsers.h"
#include "Image.h"

namespace sfui {


class Panel: public ComponentBase {
public:
    Panel(ComponentBase& parent, const nlohmann::json& json) : ComponentBase(parent, json), m_background(*this, GetBackgroundProperties(json)) {
        // parse optional properties
        if (json.contains("background-image") && (!json.contains("width") || !json.contains("height"))) {
            auto [width, height] = m_background.GetNativeSize();
            if (!json.contains("width")) SetWidth(width);
            if (!json.contains("height")) SetHeight(height);
        }
        //OnResize([this](std::uint16_t width, std::uint16_t height){ m_background.setSize(sf::Vector2f(width, height)); });
        //OnMove([this](std::int16_t x, std::int16_t y){ m_background.setPosition(x, y); });
    }
    ~Panel() = default;

    void SetBackgroundImage(const std::string& image_path) {
        m_background.SetImage(image_path);
    }

    void SetBackgroundColor(sf::Color color) {
        m_background.SetBackgroundColor(color);
    }

    void SetBackgroundColor(std::uint32_t color) {
        SetBackgroundColor(sf::Color(color));
    }

    void Render(sf::RenderWindow& window) override {
        m_background.Render(window);
    }

private:
    static nlohmann::json GetBackgroundProperties(nlohmann::json json) {
        nlohmann::json background {{"name", "_background"}, {"width", "100%"}, {"height", "100%"}};
        if (json.contains("background-image")) background += {"image", json["background-image"]};
        if (json.contains("background-color")) background += {"background-color", json["background-color"]};
        return background;
    }

private:
    Image m_background;
};

}

#endif // SFUI_PANEL_H_INCLUDED
