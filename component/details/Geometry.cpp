#include "Geometry.h"
#include "ComponentBase.h"
#include <cassert>

using namespace sfui;

ComponentGeometry::Properties::Properties(const nlohmann::json& json)
    : Component::Properties(json)
{
    if (json.contains("position"))  position    = ParsePosition(json["position"]);
    if (json.contains("left"))      left        = ParseOffset(json["left"]);
    if (json.contains("right"))     right       = ParseOffset(json["right"]);
    if (json.contains("top"))       top         = ParseOffset(json["top"]);
    if (json.contains("bottom"))    bottom      = ParseOffset(json["bottom"]);
    if (json.contains("width"))     width       = ParseSize(json["width"]);
    if (json.contains("height"))    height      = ParseSize(json["height"]);
}

ComponentGeometry::ComponentGeometry(ComponentBase& parent, const Properties& properties)
    : Component(parent, properties)
{
    if (IsRoot())                        m_position = Position::Absolute;
    if (properties.position.has_value()) m_position = properties.position.value();
    if (properties.left.has_value())     m_left     = properties.left.value();
    if (properties.right.has_value())    m_right    = properties.right.value();
    if (properties.top.has_value())      m_top      = properties.top.value();
    if (properties.bottom.has_value())   m_bottom   = properties.bottom.value();
    if (properties.width.has_value())    m_width    = properties.width.value();
    if (properties.height.has_value())   m_height   = properties.height.value();

    assert(!IsRoot() || (m_position == Position::Absolute && m_width.has_value() && m_height.has_value()));

#if 0 // Can not be called from the constructor since it calls virtual methods
    OnUpdateGeometry();
#else
    m_cachedLeft = ComputeLeft();
    m_cachedRight = ComputeRight();
    m_cachedTop = ComputeTop();
    m_cachedBottom = ComputeBottom();
    m_cachedWidth = ComputeWidth();
    m_cachedHeight = ComputeHeight();
    m_cachedAbsoluteX = ComputeAbsoluteX();
    m_cachedAbsoluteY = ComputeAbsoluteY();
#endif
}

bool ComponentGeometry::Contains(OffsetPixels x, OffsetPixels y) const
{
    return x >= AbsoluteX() && x < AbsoluteX() + Width() && y >= AbsoluteY() && y < AbsoluteY() + Height();
}

bool ComponentGeometry::Contains(std::pair<OffsetPixels, OffsetPixels> point) const
{
    return Contains(point.first, point.second);
}

void ComponentGeometry::SetLeft(OffsetPixels leftOffset)
{
    m_left = leftOffset;
    UpdateGeometry_();
}

void ComponentGeometry::SetRight(OffsetPixels rightOffset)
{
    m_right = rightOffset;
    UpdateGeometry_();
}

void ComponentGeometry::SetTop(OffsetPixels topOffset)
{
    m_top = topOffset;
    UpdateGeometry_();
}

void ComponentGeometry::SetBottom(OffsetPixels bottomOffset)
{
    m_bottom = bottomOffset;
    UpdateGeometry_();
}

void ComponentGeometry::SetWidth(SizePixels width)
{
    m_width = width;
    UpdateGeometry_();
}

void ComponentGeometry::SetHeight(SizePixels height)
{
    m_height = height;
    UpdateGeometry_();
}

void ComponentGeometry::SetSize(SizePixels width, SizePixels height)
{
    m_width = width;
    m_height = height;
    UpdateGeometry_();
}

void ComponentGeometry::SetPosition(OffsetPixels left,
                                    OffsetPixels top,
                                    std::optional<OffsetPixels> right,
                                    std::optional<OffsetPixels> bottom)
{
    m_left = left;
    m_top = top;
    if (right.has_value()) m_right = right.value();
    if (bottom.has_value()) m_bottom = bottom.value();
    UpdateGeometry_();
}

OffsetPixels ComponentGeometry::ComputeLeft() const
{
    assert(!IsRoot() || m_position == Position::Absolute);

    // Use defined left offset if possible
    if (m_left.has_value()) {
        assert(!IsRoot() || std::holds_alternative<OffsetPixels>(*m_left));
        return std::holds_alternative<OffsetPixels>(*m_left) ? std::get<OffsetPixels>(*m_left)
            : std::get<OffsetPercentage>(*m_left) * ParentWidth();
    }

    // Root object always returns zero left offset
    if (IsRoot()) {
        return 0;
    }

    // Try to calculate left offset using right alignment
    if (m_right.has_value() && m_width.has_value()) {
        return ParentWidth() - ComputeRight() - ComputeWidth();
    }

    // Try to calculate left offset using center alignment
    if (m_width.has_value()) {
        return (ParentWidth() - ComputeWidth()) / 2;
    }

    // Width is not defined, so component will use zero left offset
    return 0;
}

OffsetPixels ComponentGeometry::ComputeRight() const
{
    assert(!IsRoot() || m_position == Position::Absolute);

    // Use defined right offset if possible
    if (m_right.has_value()) {
        assert(!IsRoot() || std::holds_alternative<OffsetPixels>(*m_right));
        return std::holds_alternative<OffsetPixels>(*m_right) ? std::get<OffsetPixels>(*m_right)
            : std::get<OffsetPercentage>(*m_right) * ParentWidth();
    }

    // Root object always returns zero right offset
    if (IsRoot()) {
        return 0;
    }

    // Try to calculate right offset using left alignment
    if (m_left.has_value() && m_width.has_value()) {
        return ParentWidth() - ComputeLeft() - ComputeWidth();
    }

    // Try to calculate right offset using center alignment
    if (m_width.has_value()) {
        return (ParentWidth() - ComputeWidth()) / 2;
    }

    // Width is not defined, so component will use zero right offset
    return 0;
}

OffsetPixels ComponentGeometry::ComputeTop() const
{
    assert(!IsRoot() || m_position == Position::Absolute);

    // Use defined top offset if possible
    if (m_top.has_value()) {
        assert(!IsRoot() || std::holds_alternative<OffsetPixels>(*m_top));
        return std::holds_alternative<OffsetPixels>(*m_top) ? std::get<OffsetPixels>(*m_top)
            : std::get<OffsetPercentage>(*m_top) * ParentHeight();
    }

    // Root object always returns zero top offset
    if (IsRoot()) {
        return 0;
    }

    // Try to calculate top offset using bottom alignment
    if (m_bottom.has_value() && m_height.has_value()) {
        return ParentHeight() - ComputeBottom() - ComputeHeight();
    }

    // Try to calculate top offset using center alignment
    if (m_height.has_value()) {
        return (ParentHeight() - ComputeHeight()) / 2;
    }

    // Height is not defined, so component will use zero top offset
    return 0;
}

OffsetPixels ComponentGeometry::ComputeBottom() const
{
    assert(!IsRoot() || m_position == Position::Absolute);

    // Use defined bottom offset if possible
    if (m_bottom.has_value()) {
        assert(!IsRoot() || std::holds_alternative<OffsetPixels>(*m_bottom));
        return std::holds_alternative<OffsetPixels>(*m_bottom) ? std::get<OffsetPixels>(*m_bottom)
            : std::get<OffsetPercentage>(*m_bottom) * ParentHeight();
    }

    // Root object always returns zero bottom offset
    if (IsRoot()) {
        return 0;
    }

    // Try to calculate bottom offset using top alignment
    if (m_top.has_value() && m_height.has_value()) {
        return ParentHeight() - ComputeTop() - ComputeHeight();
    }

    // Try to calculate bottom offset using center alignment
    if (m_height.has_value()) {
        return (ParentHeight() - ComputeHeight()) / 2;
    }

    // Height is not defined, so component will use zero bottom offset
    return 0;
}

SizePixels ComponentGeometry::ComputeWidth() const
{
    assert(!IsRoot() || m_position == Position::Absolute);

    // Use left and right offset when they are defined
    if (!IsRoot() && m_left.has_value() && m_right.has_value()) {
        return ParentWidth() - ComputeLeft() - ComputeRight();
    }

    // Use defined width value
    if (m_width.has_value()) {
        assert(!IsRoot() || std::holds_alternative<SizePixels>(*m_width));
        return std::holds_alternative<SizePixels>(*m_width) ? std::get<SizePixels>(*m_width)
            : std::get<OffsetPercentage>(*m_width) * ParentWidth();
    }

    if (IsRoot()) {
        throw std::runtime_error("Width value is not defined for root component");
    }

    // Use parent width when it wasn't setup
    return ParentWidth();
}

SizePixels ComponentGeometry::ComputeHeight() const
{
    assert(!IsRoot() || m_position == Position::Absolute);

    // Use top and bottom offset when they are defined
    if (!IsRoot() && m_top.has_value() && m_bottom.has_value()) {
        return ParentHeight() - ComputeTop() - ComputeBottom();
    }

    // Use defined height value
    if (m_height.has_value()) {
        assert(!IsRoot() || std::holds_alternative<SizePixels>(*m_height));
        return std::holds_alternative<SizePixels>(*m_height) ? std::get<SizePixels>(*m_height)
            : std::get<OffsetPercentage>(*m_height) * ParentHeight();
    }

    if (IsRoot()) {
        throw std::runtime_error("Height value is not defined for root component");
    }

    // Use parent height when it wasn't setup
    return ParentHeight();
}

OffsetPixels ComponentGeometry::ComputeAbsoluteX() const
{
    return m_position == Position::Absolute ? Left() : Parent().AbsoluteX() + Left();
}

OffsetPixels ComponentGeometry::ComputeAbsoluteY() const
{
    return m_position == Position::Absolute ? Top() : Parent().AbsoluteY() + Top();
}

SizePixels ComponentGeometry::ParentWidth() const
{
    assert(!IsRoot());
    return m_position == Position::Absolute ? Root().Width() : Parent().Width();
}

SizePixels ComponentGeometry::ParentHeight() const
{
    assert(!IsRoot());
    return m_position == Position::Absolute ? Root().Height() : Parent().Height();
}


Position ComponentGeometry::ParsePosition(const nlohmann::json& json)
{
    std::string position_string = json.get<std::string>();
    return position_string == "absolute" ? Position::Absolute
        : position_string == "relative" ? Position::Relative
        : throw std::runtime_error("Error parsing component position");
}

std::variant<OffsetPixels, OffsetPercentage> ComponentGeometry::ParseOffset(const nlohmann::json& json)
{
    if (json.is_number()) {
        return json.get<OffsetPixels>();
    } else {
        std::string string = json.get<std::string>();
        if (auto n = string.find("%"); n != std::string::npos) {
            return std::stof(string.substr(0, n)) / 100.0f; // expected string format is "25%" or "100.0%"
        } else {
            throw std::runtime_error("Error parsing component offset: " + string);
        }
    }
}

std::variant<SizePixels, SizePercentage> ComponentGeometry::ParseSize(const nlohmann::json& json)
{
    if (json.is_number()) {
        return json.get<SizePixels>();
    } else {
        std::string string = json.get<std::string>();
        if (auto n = string.find("%"); n != std::string::npos) {
            return std::stof(string.substr(0, n)) / 100.0f; // expected string format is "25%" or "100.0%"
        } else {
            throw std::runtime_error("Error parsing component size: " + string);
        }
    }
}

void ComponentGeometry::OnUpdateGeometry()
{
    // Compute a new relative position
    m_cachedLeft = ComputeLeft();
    m_cachedRight = ComputeRight();
    m_cachedTop = ComputeTop();
    m_cachedBottom = ComputeBottom();

    // Compute a new absolute X,Y position
    const auto x = ComputeAbsoluteX();
    const auto y = ComputeAbsoluteY();
    const bool isMoved = (m_cachedAbsoluteX != x || m_cachedAbsoluteY != y);
    m_cachedAbsoluteX = x;
    m_cachedAbsoluteY = y;

    // Compute a new dimensions
    const auto width = ComputeWidth();
    const auto height = ComputeHeight();
    const bool isResized = (m_cachedWidth != width || m_cachedHeight != height);
    m_cachedWidth = width;
    m_cachedHeight = height;

    // Call corresponded handlers
    if (isMoved) OnMove();
    if (isResized) OnResize();
}
