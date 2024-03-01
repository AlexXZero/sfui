#include "Component.h"
#include "ComponentBase.h"
#include "Handlers.h"
#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>
#include <mutex>

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

using namespace sfui;

Component::Component(ComponentBase& parent, const nlohmann::json& json)
    : std::enable_shared_from_this<Component>(), m_parent(parent), m_name(json["name"]), m_enabled(true), m_visible(true)
{
    if (json.contains("enabled")) m_enabled = json["enabled"].get<bool>();
    if (json.contains("visible")) m_visible = json["visible"].get<bool>();
}

const std::string& Component::Name() const
{
    return m_name;
}

ComponentBase& Component::Parent() const
{
    return m_parent;
}

ComponentBase& Component::Root() const
{
    return IsRoot() ? m_parent : m_parent.Root();
}

bool Component::IsRoot() const
{
    return this == &m_parent;
}

bool Component::IsEnabled() const
{
    return m_enabled;
}

void Component::Enable()
{
    m_enabled = true;
    OnEnable();
}

void Component::Disable()
{
    m_enabled = false;
    OnDisable();
}

bool Component::IsVisible() const
{
    return m_visible;
}

void Component::Show()
{
    m_visible = true;
    OnShow();
}

void Component::Hide()
{
    m_visible = false;
    OnHide();
}

static std::weak_ptr<Component> g_focusedComponent;
static std::mutex g_focusControlMutex;

std::shared_ptr<ComponentBase> Component::FocusedComponent()
{
    return std::dynamic_pointer_cast<ComponentBase>(g_focusedComponent.lock());
}

bool Component::IsFocused() const
{
    auto focusedComponent_sp = FocusedComponent();
    return focusedComponent_sp != nullptr && focusedComponent_sp.get() == this;
}

void Component::GainFocus()
{
    std::lock_guard guard(g_focusControlMutex);
    if (IsFocused()) return;

    // Release focused earlier another component
    if (auto focusedComponent_sp = g_focusedComponent.lock()) {
        focusedComponent_sp->OnLoseFocus();
    }

    g_focusedComponent = shared_from_this();
    OnGainFocus();
}

void Component::LoseFocus()
{
    std::lock_guard guard(g_focusControlMutex);
    if (IsFocused()) {
        OnLoseFocus();
        g_focusedComponent.reset();
    }
}

void Component::BringToFront()
{
    if (IsRoot()) return; // The root element is always at the front and back, so no need to bring it forward.
    auto predicate = [this](const std::shared_ptr<Component> component){ return component.get() == this; };
    auto& component_list = static_cast<Component&>(Parent()).m_components;
    auto it = std::find_if(component_list.rbegin(), component_list.rend(), predicate);
    assert(it != component_list.rend());
    std::rotate(component_list.rbegin(), it, std::next(it));
}

void Component::BringToBack()
{
    if (IsRoot()) return; // The root element is always at the front and back, so no need to send it backward.
    auto predicate = [this](const std::shared_ptr<Component> component){ return component.get() == this; };
    auto& component_list = static_cast<Component&>(Parent()).m_components;
    auto it = std::find_if(component_list.begin(), component_list.end(), predicate);
    assert(it != component_list.end());
    std::rotate(component_list.begin(), it, std::next(it));
}

ComponentBase& Component::operator[](std::string_view name)
{
    auto name_end = name.find(".");
    if (name_end == std::string::npos) {
        if (name == "this") return dynamic_cast<ComponentBase&>(*this);
        if (name == Root().Name()) return Root();
        for (auto& component_sp: m_components) {
            if (component_sp->Name() == name) {
                return *component_sp;
            }
        }
        throw std::out_of_range(Name() + ": " + std::string(name) + " component is not found");
    }

    return operator[](name.substr(0u, name_end)).operator[](name.substr(name_end + 1));
}

void Component::Render_(sf::RenderWindow& window)
{
    Render(window);
    OnRender(window);

    for (auto& component_sp: m_components) {
        if (component_sp->IsVisible()) {
            component_sp->Render_(window);
        }
    }
}

bool Component::HandleEvent_(const sf::Event& event)
{
    for (auto& component_sp: std::ranges::reverse_view(m_components)) {
        if (component_sp->IsVisible() && component_sp->HandleEvent_(event)) {
            return true;
        }
    }

    return HandleEvent(event);
}

void Component::Update_()
{
    OnUpdate();

    for (auto& component_sp: m_components) {
        if (component_sp->IsVisible()) {
            component_sp->Update_();
        }
    }
}
