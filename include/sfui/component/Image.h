#ifndef SFUI_IMAGE_H_INCLUDED
#define SFUI_IMAGE_H_INCLUDED

#include "details/ComponentBase.h"
#include "../ImageData.h"

namespace sfui {

class Image: public ComponentBase {
public:
    struct Properties: public ComponentBase::Properties {
        std::optional<sf::Color> backgroundColor;
        std::optional<std::string> imagePath;
        std::optional<std::variant<OffsetPixels, OffsetPercentage>> scrollSpeed;

        Properties() = default;
        Properties(const nlohmann::json& json);
    };

    Image(ComponentBase& parent, const Properties& properties);
    Image(ComponentBase& parent, const nlohmann::json& json)
        : Image(parent, Properties(json)) { ParseHandlers(json); }
    ~Image() = default;

    void SetBackgroundColor(sf::Color color);
    void SetBackgroundColor(std::uint32_t color) {
        SetBackgroundColor(sf::Color(color));
    }

    void SetImage(const std::string& image_path);

    void SetScrollSpeed(std::variant<OffsetPixels, OffsetPercentage> speed);

    std::pair<uint16_t, uint16_t> GetNativeSize() const;

    void Render(sf::RenderWindow& window) override;

private:
    std::optional<RepeatingAnimation> m_image;
    std::optional<sf::RectangleShape> m_background;
    std::optional<OffsetPercentage> m_scrollSpeed;
    std::chrono::high_resolution_clock::time_point m_scrollStart;
};

}

#endif // SFUI_IMAGE_H_INCLUDED
