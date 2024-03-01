#ifndef SFUI_COMPONENT_GEOMETRY_H_INCLUDED
#define SFUI_COMPONENT_GEOMETRY_H_INCLUDED

#include "Component.h"
#include <nlohmann/json.hpp>
#include <optional>
#include <variant>

namespace sfui {

enum class Position { Relative, Absolute };
using OffsetPixels      = std::int16_t;
using OffsetPercentage  = float;
using SizePixels        = std::uint16_t;
using SizePercentage    = float;

class ComponentGeometry : public Component {
public:
    ComponentGeometry(ComponentBase& parent, const nlohmann::json& json);
    virtual ~ComponentGeometry() = default;

    // Getters
    OffsetPixels Left() const;
    OffsetPixels Right() const;
    OffsetPixels Top() const;
    OffsetPixels Bottom() const;
    SizePixels Width() const;
    SizePixels Height() const;

    // Global horizontal and vertical position
    OffsetPixels AbsoluteX() const;
    OffsetPixels AbsoluteY() const;

    // Check if a point is inside the component's area.
    bool Contains(OffsetPixels x, OffsetPixels y) const;
    bool Contains(std::pair<OffsetPixels, OffsetPixels> point) const;

    // Setters
    void SetLeft(OffsetPixels leftOffset);
    void SetRight(OffsetPixels rightOffset);
    void SetTop(OffsetPixels topOffset);
    void SetBottom(OffsetPixels bottomOffset);
    void SetWidth(SizePixels width);
    void SetHeight(SizePixels height);

private:
    // Getters for the parent's dimensions
    SizePixels ParentWidth() const;
    SizePixels ParentHeight() const;

    // Parsers
    static Position ParsePosition(const nlohmann::json& json);
    static std::variant<OffsetPixels, OffsetPercentage> ParseOffset(const nlohmann::json& json);
    static std::variant<SizePixels, SizePercentage> ParseSize(const nlohmann::json& json);

    virtual void OnResize() = 0;
    virtual void OnMove() = 0;

private:
    Position m_position = Position::Relative;
    std::optional<std::variant<SizePixels, SizePercentage>> m_width, m_height;
    std::optional<std::variant<OffsetPixels, OffsetPercentage>> m_left, m_right, m_top, m_bottom;
};

}

#endif // SFUI_COMPONENT_GEOMETRY_H_INCLUDED
