#include "Geometry.h"
#include "Container.h"
#include <cassert>

using namespace sfui;

ComponentGeometry::ComponentGeometry(ComponentContainer& parent, const nlohmann::json& json)
        : ComponentNode(parent, json)
{
    if (IsRoot())                   m_position  = Position::Absolute;
    if (json.contains("position"))  m_position  = ParsePosition(json["position"]);
    if (json.contains("left"))      m_left      = ParseOffset(json["left"]);
    if (json.contains("right"))     m_right     = ParseOffset(json["right"]);
    if (json.contains("top"))       m_top       = ParseOffset(json["top"]);
    if (json.contains("bottom"))    m_bottom    = ParseOffset(json["bottom"]);
    if (json.contains("width"))     m_width     = ParseSize(json["width"]);
    if (json.contains("height"))    m_height    = ParseSize(json["height"]);

    assert(!IsRoot() || (m_position == Position::Absolute && m_width.has_value() && m_height.has_value()));
}

ComponentGeometry::OffsetPixels ComponentGeometry::Left() const
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
        return ParentWidth() - Right() - Width();
    }

    // Try to calculate left offset using center alignment
    if (m_width.has_value()) {
        return (ParentWidth() - Width()) / 2;
    }

    // Width is not defined, so component will use zero left offset
    return 0;
}

ComponentGeometry::OffsetPixels ComponentGeometry::Right() const
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
        return ParentWidth() - Left() - Width();
    }

    // Try to calculate right offset using center alignment
    if (m_width.has_value()) {
        return (ParentWidth() - Width()) / 2;
    }

    // Width is not defined, so component will use zero right offset
    return 0;
}

ComponentGeometry::OffsetPixels ComponentGeometry::Top() const
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
        return ParentHeight() - Bottom() - Height();
    }

    // Try to calculate top offset using center alignment
    if (m_height.has_value()) {
        return (ParentHeight() - Height()) / 2;
    }

    // Height is not defined, so component will use zero top offset
    return 0;
}

ComponentGeometry::OffsetPixels ComponentGeometry::Bottom() const
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
        return ParentHeight() - Top() - Height();
    }

    // Try to calculate bottom offset using center alignment
    if (m_height.has_value()) {
        return (ParentHeight() - Height()) / 2;
    }

    // Height is not defined, so component will use zero bottom offset
    return 0;
}

ComponentGeometry::SizePixels ComponentGeometry::Width() const
{
    assert(!IsRoot() || m_position == Position::Absolute);

    // Use left and right offset when they are defined
    if (!IsRoot() && m_left.has_value() && m_right.has_value()) {
        return ParentWidth() - Left() - Right();
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

ComponentGeometry::SizePixels ComponentGeometry::Height() const
{
    assert(!IsRoot() || m_position == Position::Absolute);

    // Use top and bottom offset when they are defined
    if (!IsRoot() && m_top.has_value() && m_bottom.has_value()) {
        return ParentHeight() - Top() - Bottom();
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

ComponentGeometry::OffsetPixels ComponentGeometry::AbsoluteX() const
{
    return m_position == Position::Absolute ? Left() : Parent().AbsoluteX() + Left();
}

ComponentGeometry::OffsetPixels ComponentGeometry::AbsoluteY() const
{
    return m_position == Position::Absolute ? Top() : Parent().AbsoluteY() + Top();
}

bool ComponentGeometry::Contains(OffsetPixels x, OffsetPixels y) const
{
    return x >= AbsoluteX() && x < AbsoluteX() + Width() && y >= AbsoluteY() && y < AbsoluteY() + Height();
}

bool ComponentGeometry::Contains(std::pair<OffsetPixels, OffsetPixels> point) const
{
    return Contains(point.first, point.second);
}

ComponentGeometry::SizePixels ComponentGeometry::ParentWidth() const
{
    assert(!IsRoot());
    return m_position == Position::Absolute ? Root().Width() : Parent().Width();
}

ComponentGeometry::SizePixels ComponentGeometry::ParentHeight() const
{
    assert(!IsRoot());
    return m_position == Position::Absolute ? Root().Height() : Parent().Height();
}


ComponentGeometry::Position ComponentGeometry::ParsePosition(const nlohmann::json& json)
{
    std::string position_string = json.get<std::string>();
    return position_string == "absolute" ? Position::Absolute
        : position_string == "relative" ? Position::Relative
        : throw std::runtime_error("Error parsing component position");
}

std::variant<ComponentGeometry::OffsetPixels, ComponentGeometry::OffsetPercentage> ComponentGeometry::ParseOffset(const nlohmann::json& json)
{
    if (json.is_number()) {
        return json.get<ComponentGeometry::OffsetPixels>();
    } else {
        std::string string = json.get<std::string>();
        if (auto n = string.find("%"); n != std::string::npos) {
            return std::stof(string.substr(0, n)) / 100.0f; // expected string format is "25%" or "100.0%"
        } else {
            throw std::runtime_error("Error parsing component offset: " + string);
        }
    }
}

std::variant<ComponentGeometry::SizePixels, ComponentGeometry::SizePercentage> ComponentGeometry::ParseSize(const nlohmann::json& json)
{
    if (json.is_number()) {
        return json.get<ComponentGeometry::SizePixels>();
    } else {
        std::string string = json.get<std::string>();
        if (auto n = string.find("%"); n != std::string::npos) {
            return std::stof(string.substr(0, n)) / 100.0f; // expected string format is "25%" or "100.0%"
        } else {
            throw std::runtime_error("Error parsing component size: " + string);
        }
    }
}
