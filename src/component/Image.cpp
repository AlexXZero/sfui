#include "component/Image.h"
#include "component/details/Parsers.h"

using namespace sfui;

Image::Properties::Properties(const nlohmann::json& json)
    : ComponentBase::Properties(json)
{
    if (json.contains("image")) imagePath = json["image"].get<std::string>();
    if (json.contains("background-color")) backgroundColor = ParseColor(json["background-color"]);
    if (json.contains("scrollSpeed")) scrollSpeed = ParseOffset(json["scrollSpeed"]);
}

Image::Image(ComponentBase& parent, const Properties& properties)
    : ComponentBase(parent, properties)
{
    if (properties.imagePath.has_value()) {
        SetImage(properties.imagePath.value());
        auto [width, height] = m_image->GetNativeSize();
        if (!properties.width.has_value()) SetWidth(width);
        if (!properties.height.has_value()) SetHeight(height);
        m_image->SetSize({float(Width()), float(Height())});
        m_image->SetPosition(AbsoluteX(), AbsoluteY());
    }

    if (properties.backgroundColor.has_value()) {
        SetBackgroundColor(properties.backgroundColor.value());
    }

    if (properties.scrollSpeed.has_value()) {
        SetScrollSpeed(properties.scrollSpeed.value());
    }

    LinkEvent(OnResize([this]{
        if (m_image.has_value()) m_image->SetSize(sf::Vector2f(Width(), Height()));
        if (m_background.has_value()) m_background->setSize(sf::Vector2f(Width(), Height()));
    }));
    LinkEvent(OnMove([this]{
        if (m_image.has_value()) m_image->SetPosition(AbsoluteX(), AbsoluteY());
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
    m_image.emplace(image_path);
}

void Image::SetScrollSpeed(std::variant<OffsetPixels, OffsetPercentage> speed)
{
    if (std::holds_alternative<OffsetPixels>(speed) && std::get<OffsetPixels>(speed) == 0) {
        m_scrollSpeed = std::nullopt;
    } else {
        m_scrollSpeed = std::holds_alternative<OffsetPixels>(speed)
                      ? OffsetPercentage(Width() / std::get<OffsetPixels>(speed))
                      : std::get<OffsetPercentage>(speed);
        m_scrollStart = std::chrono::high_resolution_clock::now();
    }
}

std::pair<uint16_t, uint16_t> Image::GetNativeSize() const
{
    auto [width, height] = m_image->GetNativeSize();
    return {width, height};
}

void Image::Render(sf::RenderWindow& window)
{
    if (m_background.has_value()) {
        window.draw(m_background.value());
    }
    if (m_image.has_value()) {
        if (m_scrollSpeed.has_value()) {
            constexpr auto cMillisecondsPerSecond = 1000.0f;
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_scrollStart).count();
            auto offset = std::fmod(Width() * m_scrollSpeed.value() * elapsedMilliseconds / cMillisecondsPerSecond, Width());
            if (offset > 0) offset -= Width();

            // First draw
            auto position = m_image.value().GetPosition();
            position.x = offset;
            m_image.value().SetPosition(position);
            window.draw(m_image.value());

            // Second draw
            position.x += Width();
            m_image.value().SetPosition(position);
            window.draw(m_image.value());
        } else {
            window.draw(m_image.value());
        }
    }
}
