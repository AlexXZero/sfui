#include "Container.h"
#include "Parsers.h"

using namespace sfui;

#if __cplusplus >= 202002L // `std::ranges::reverse_view` support
#include <ranges>
#else
namespace std::ranges {
template <typename Container>
class reverse_view {
public:
    constexpr reverse_view(const Container& container) : base_(container) {}
    constexpr auto begin() const { return base_.rbegin(); }
    constexpr auto end() const { return base_.rend(); }
private:
    const Container& base_;
};
}
#endif

ComponentContainer::ComponentContainer(Component& parent, const nlohmann::json& json)
        : ComponentBase(parent, json)
{
    ParseComponents(*this, json);
}

void ComponentContainer::BringToFront(const Component& component)
{
    auto predicate = [&component](const auto& c){ return c.get() == &component; };
    auto it = std::find_if(m_components.rbegin(), m_components.rend(), predicate);
    assert(it != m_components.rend());
    std::rotate(m_components.rbegin(), it, std::next(it));
}

void ComponentContainer::BringToBack(const Component& component)
{
    auto predicate = [&component](const auto& c){ return c.get() == &component; };
    auto it = std::find_if(m_components.begin(), m_components.end(), predicate);
    assert(it != m_components.end());
    std::rotate(m_components.begin(), it, std::next(it));
}

void ComponentContainer::Render_(sf::RenderWindow& window)
{
    ComponentBase::Render_(window);

    for (auto& component_up: m_components) {
        if (component_up->IsVisible()) {
            component_up->Render_(window);
        }
    }
}

bool ComponentContainer::HandleEvent_(const sf::Event& event)
{
    for (auto& component_up: std::ranges::reverse_view(m_components)) {
        if (component_up->IsVisible() && component_up->HandleEvent_(event)) {
            return true;
        }
    }

    return ComponentBase::HandleEvent_(event);
}

Component& ComponentContainer::operator[](std::string_view name)
{
    auto name_end = name.find(".");
    if (name_end == std::string::npos) {
        if (name == "this") return *this;
        if (name == Root().Name()) return Root();
        for (auto& component_up: m_components) {
            if (component_up->Name() == name) {
                return *component_up;
            }
        }
        throw std::out_of_range(Name() + ": " + std::string(name) + " component is not found");
    }

    return dynamic_cast<ComponentContainer&>(operator[](name.substr(0u, name_end))).operator[](name.substr(name_end + 1));
}
