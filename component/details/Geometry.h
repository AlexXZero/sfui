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
    struct Properties : public Component::Properties {
        std::optional<Position> position;
        std::optional<std::variant<SizePixels, SizePercentage>> width, height;
        std::optional<std::variant<OffsetPixels, OffsetPercentage>> left, right, top, bottom;

        Properties() = default;
        Properties(const nlohmann::json& json);
    };

    ComponentGeometry(ComponentBase& parent, const Properties& properties);
    virtual ~ComponentGeometry() = default;

    // Getters
    OffsetPixels Left() const   { return m_cachedLeft; }
    OffsetPixels Right() const  { return m_cachedRight; }
    OffsetPixels Top() const    { return m_cachedTop; }
    OffsetPixels Bottom() const { return m_cachedBottom; }
    SizePixels Width() const    { return m_cachedWidth; }
    SizePixels Height() const   { return m_cachedHeight; }

    // Global horizontal and vertical position
    OffsetPixels AbsoluteX() const  { return m_cachedAbsoluteX; }
    OffsetPixels AbsoluteY() const  { return m_cachedAbsoluteY; }

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
    void SetSize(SizePixels width, SizePixels height);
    void SetPosition(OffsetPixels left, OffsetPixels top, std::optional<OffsetPixels> right = std::nullopt, std::optional<OffsetPixels> bottom = std::nullopt);

private:
    // Computers for computing cached values
    OffsetPixels ComputeLeft() const;
    OffsetPixels ComputeRight() const;
    OffsetPixels ComputeTop() const;
    OffsetPixels ComputeBottom() const;
    SizePixels ComputeWidth() const;
    SizePixels ComputeHeight() const;
    OffsetPixels ComputeAbsoluteX() const;
    OffsetPixels ComputeAbsoluteY() const;

    // Getters for the parent's dimensions
    SizePixels ParentWidth() const;
    SizePixels ParentHeight() const;

    // Parsers
    static Position ParsePosition(const nlohmann::json& json);
    static std::variant<OffsetPixels, OffsetPercentage> ParseOffset(const nlohmann::json& json);
    static std::variant<SizePixels, SizePercentage> ParseSize(const nlohmann::json& json);

    virtual void OnUpdateGeometry() final;
    virtual void OnResize() = 0;
    virtual void OnMove() = 0;

private:
    Position m_position = Position::Relative;
    std::optional<std::variant<SizePixels, SizePercentage>> m_width, m_height;
    std::optional<std::variant<OffsetPixels, OffsetPercentage>> m_left, m_right, m_top, m_bottom;

    // Cashed component position and dimensions
    SizePixels m_cachedWidth, m_cachedHeight;
    OffsetPixels m_cachedAbsoluteX, m_cachedAbsoluteY;
    OffsetPixels m_cachedLeft, m_cachedRight, m_cachedTop, m_cachedBottom;
};

}

#endif // SFUI_COMPONENT_GEOMETRY_H_INCLUDED
