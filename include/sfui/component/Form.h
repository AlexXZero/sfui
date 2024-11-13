#ifndef SFUI_FORM_H_INCLUDED
#define SFUI_FORM_H_INCLUDED

#include "details/ComponentBase.h"
#include "details/Parsers.h"
#include "Image.h"

namespace sfui {

class Form: public ComponentBase {
public:
    Form(ComponentBase& parent, const nlohmann::json& json) : ComponentBase(parent, json), m_background(*this, GetBackgroundProperties(json)) {
        // parse optional properties
        if (json.contains("background-image") && (!json.contains("width") || !json.contains("height"))) {
            auto [width, height] = m_background.GetNativeSize();
            if (!json.contains("width")) SetWidth(width);
            if (!json.contains("height")) SetHeight(height);
        }
        LinkEvent(OnResize([this]{ m_background.SetSize(Width(), Height()); }));
        LinkEvent(OnMove([this]{ m_background.SetPosition(0, 0); }));
        LinkEvent(OnMouseClick([this]{ BringToFront(); }));
    }
    ~Form() = default;

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
        nlohmann::json background {{"name", "_background"}};
        if (json.contains("background-image")) background += {"image", json["background-image"]};
        if (json.contains("background-color")) background += {"background-color", json["background-color"]};
        return background;
    }

private:
    Image m_background;
};

}

#endif // SFUI_FORM_H_INCLUDED
