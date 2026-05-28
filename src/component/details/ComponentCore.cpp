#include "component/details/ComponentCore.h"
#include "component/details/ComponentBase.h"
#include "component/details/Handlers.h"
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

ComponentCore::Properties::Properties(ConfigView config, const Properties& defaults)
    : name{config.required<std::string>("name")}
    , isEnabled{config.valueOr<bool>("enabled", defaults.isEnabled)}
    , isVisible{config.valueOr<bool>("visible", defaults.isVisible)}
    , isIgnorable{config.valueOr<bool>("ignorable", defaults.isIgnorable)}
{}

ComponentCore::ComponentCore(ComponentBase& parent, const ComponentCore::Properties& properties)
    : std::enable_shared_from_this<ComponentCore>()
    , m_parent(parent)
    , m_ui(parent.m_ui)
    , m_name(properties.name)
    , m_enabled(properties.isEnabled)
    , m_visible(properties.isVisible)
    , m_ignorable(properties.isIgnorable)
{
}

ComponentCore::ComponentCore(UserInterface& uiContext, const Properties& properties)
    : std::enable_shared_from_this<ComponentCore>()
    , m_parent(static_cast<ComponentBase&>(*this))
    , m_ui(uiContext)
    , m_name(properties.name)
    , m_enabled(properties.isEnabled)
    , m_visible(properties.isVisible)
    , m_ignorable(properties.isIgnorable)
{
}

void ComponentCore::Remove(const std::shared_ptr<ComponentCore> component)
{
    auto it = std::find(m_components.begin(), m_components.end(), component);
    assert(it != m_components.end());
    m_components.erase(it);
}

const std::string& ComponentCore::Name() const
{
    return m_name;
}

ComponentBase& ComponentCore::Parent() const
{
    return m_parent;
}

ComponentBase& ComponentCore::Root() const
{
    return IsRoot() ? m_parent : m_parent.Root();
}

bool ComponentCore::IsRoot() const
{
    return this == &m_parent;
}

bool ComponentCore::IsEnabled() const
{
    return m_enabled;
}

void ComponentCore::Enable()
{
    m_enabled = true;
    OnEnable();
}

void ComponentCore::Disable()
{
    m_enabled = false;
    OnDisable();
}

bool ComponentCore::IsVisible() const
{
    return m_visible;
}

void ComponentCore::Show()
{
    m_visible = true;
    OnShow();
}

void ComponentCore::Hide()
{
    m_visible = false;
    OnHide();
}

bool ComponentCore::IsIgnorable() const
{
    return m_ignorable;
}

void ComponentCore::Ignore()
{
    m_ignorable = true;
}

void ComponentCore::Intercept()
{
    m_ignorable = false;
}

static std::weak_ptr<ComponentCore> g_focusedComponent;
static std::mutex g_focusControlMutex;

std::shared_ptr<ComponentBase> ComponentCore::FocusedComponent()
{
    return std::dynamic_pointer_cast<ComponentBase>(g_focusedComponent.lock());
}

bool ComponentCore::IsFocused() const
{
    auto focusedComponent_sp = FocusedComponent();
    return focusedComponent_sp != nullptr && focusedComponent_sp.get() == this;
}

void ComponentCore::GainFocus()
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

void ComponentCore::LoseFocus()
{
    std::lock_guard guard(g_focusControlMutex);
    if (IsFocused()) {
        OnLoseFocus();
        g_focusedComponent.reset();
    }
}

void ComponentCore::RotateFocus()
{
    std::lock_guard guard(g_focusControlMutex);
    auto focusedComponent_sp = FocusedComponent();
    if (focusedComponent_sp != nullptr) {
        auto& component_list = static_cast<ComponentCore&>(focusedComponent_sp->Parent()).m_components;
        auto it = std::find(component_list.begin(), component_list.end(), focusedComponent_sp);
        assert(it != component_list.end());
        auto it_next = (std::next(it) != component_list.end() ? std::next(it) : component_list.begin());

        std::static_pointer_cast<ComponentCore>(focusedComponent_sp)->OnLoseFocus();
        g_focusedComponent = *it_next;
        std::static_pointer_cast<ComponentCore>(*it_next)->OnGainFocus();
    }
}

void ComponentCore::BringToFront()
{
    if (IsRoot()) return; // The root element is always at the front and back, so no need to bring it forward.
    auto& component_list = static_cast<ComponentCore&>(Parent()).m_components;
    auto it = std::find(component_list.rbegin(), component_list.rend(), shared_from_this());
    assert(it != component_list.rend());
    std::rotate(component_list.rbegin(), it, std::next(it));
    Parent().BringToFront();
}

void ComponentCore::BringToBack()
{
    if (IsRoot()) return; // The root element is always at the front and back, so no need to send it backward.
    auto& component_list = static_cast<ComponentCore&>(Parent()).m_components;
    auto it = std::find(component_list.begin(), component_list.end(), shared_from_this());
    assert(it != component_list.end());
    std::rotate(component_list.begin(), it, std::next(it));
}

ComponentBase& ComponentCore::operator[](std::string_view name)
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

void ComponentCore::RenderSubtree(sf::RenderTarget& surface)
{
    Render(surface);
    OnRender(surface);

    sf::View oldView = surface.getView();
    for (auto& component_sp: m_components) {
        if (component_sp->IsVisible()) {
            sf::View view(sf::FloatRect(
                    component_sp->AbsoluteX(),
                    component_sp->AbsoluteY(),
                    component_sp->Width(),
                    component_sp->Height()));
            view.setViewport(sf::FloatRect(
                    float(component_sp->AbsoluteX()) / Root().Width(),
                    float(component_sp->AbsoluteY()) / Root().Height(),
                    float(component_sp->Width()) / Root().Width(),
                    float(component_sp->Height()) / Root().Height()));
            surface.setView(view); // Clip drawing to component boundaries
            component_sp->RenderSubtree(surface);
        }
    }
    surface.setView(oldView);
}

bool ComponentCore::DispatchToSubtree(const sf::Event& event)
{
    if (m_ignorable) return false;

    for (auto& component_sp: std::ranges::reverse_view(m_components)) {
        if (component_sp->IsVisible() && component_sp->DispatchToSubtree(event)) {
            return true;
        }
    }

    return HandleEvent(event);
}

void ComponentCore::UpdateSubtree()
{
    OnUpdate();

    for (auto& component_sp: m_components) {
        if (component_sp->IsVisible()) {
            component_sp->UpdateSubtree();
        }
    }
}

void ComponentCore::UpdateGeometry_()
{
    OnUpdateGeometry();

    for (auto& component_sp: m_components) {
        component_sp->UpdateGeometry_();
    }
}
