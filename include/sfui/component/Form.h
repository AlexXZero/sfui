#ifndef SFUI_FORM_H_INCLUDED
#define SFUI_FORM_H_INCLUDED

#include "details/ComponentBase.h"
#include "details/Parsers.h"
#include "Image.h"

namespace sfui {

class Form: public ComponentBase {
public:
    static constexpr std::string_view ComponentTypeName = "form";
public:
    Form(ComponentBase& parent, ConfigView config) : ComponentBase(parent, config), m_background(*this, GetBackgroundProperties(config)) {
        // parse optional properties
        if (config.contains("background-image") && (!config.contains("width") || !config.contains("height"))) {
            auto [width, height] = m_background.GetNativeSize();
            if (!config.contains("width")) SetWidth(width);
            if (!config.contains("height")) SetHeight(height);
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
    static Image::Properties GetBackgroundProperties(ConfigView config) {
        nlohmann::json background = {{"name", "_background"}};
        if (auto backgroundImage = config.optional("background-image")) background += {"image", backgroundImage->raw()};
        if (auto backgroundColor = config.optional("background-color")) background += {"background-color", backgroundColor->raw()};
        return Image::Properties(ConfigView(background));
    }

private:
    Image m_background;
};

}

#endif // SFUI_FORM_H_INCLUDED
