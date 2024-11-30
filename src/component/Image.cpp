#include "component/Image.h"
#include "component/details/Parsers.h"

using namespace sfui;

Image::Properties::Properties(const nlohmann::json& json)
    : ComponentBase::Properties(json)
{
    if (json.contains("image")) imagePath = json["image"].get<std::string>();
    if (json.contains("background-color")) backgroundColor = ParseColor(json["background-color"]);
}

Image::Image(ComponentBase& parent, const Properties& properties)
    : ComponentBase(parent, properties)
{
    if (properties.imagePath.has_value()) {
        SetImage(properties.imagePath.value());
        auto [width, height] = m_textures.Size();
        if (!properties.width.has_value()) SetWidth(width);
        if (!properties.height.has_value()) SetHeight(height);
        m_image->setSize({float(Width()), float(Height())});
        m_image->setPosition(AbsoluteX(), AbsoluteY());
    }

    if (properties.backgroundColor.has_value()) {
        SetBackgroundColor(properties.backgroundColor.value());
    }

    LinkEvent(OnResize([this]{
        if (m_image.has_value()) m_image->setSize(sf::Vector2f(Width(), Height()));
        if (m_background.has_value()) m_background->setSize(sf::Vector2f(Width(), Height()));
    }));
    LinkEvent(OnMove([this]{
        if (m_image.has_value()) m_image->setPosition(AbsoluteX(), AbsoluteY());
        if (m_background.has_value()) m_background->setPosition(AbsoluteX(), AbsoluteY());
    }));
}

void Image::SetBackgroundColor(sf::Color color)
{
    m_background.emplace(sf::Vector2f(Width(), Height()));
    m_background->setFillColor(sf::Color(color));
    m_background->setPosition(AbsoluteX(), AbsoluteY());
}

void Image::SetImage(const std::string& image_path)
{
    m_image.emplace();
    m_textures = ImageData::Load(image_path);
    m_image->setTexture(&m_textures.Frame(0));
}

std::pair<uint16_t, uint16_t> Image::GetNativeSize() const
{
    auto [width, height] = m_textures.Size();
    return {width, height};
}

void Image::Render(sf::RenderWindow& window)
{
    if (m_background.has_value()) {
        window.draw(m_background.value());
    }
    if (m_image.has_value()) {
        window.draw(m_image.value());
    }
}
