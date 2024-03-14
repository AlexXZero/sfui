#ifndef SFUI_COMPONENT_H_INCLUDED
#define SFUI_COMPONENT_H_INCLUDED

#include <memory>
#include <vector>
#include <string>
#include <string_view>
#include <nlohmann/json.hpp>

namespace sf {
    class RenderWindow;
    class Event;
}

namespace sfui {

class ComponentBase;

class Component: public std::enable_shared_from_this<Component> {
public:
    struct Properties {
        std::string name;
        bool isEnabled = true;
        bool isVisible = true;

        Properties() = default;
        Properties(const nlohmann::json& json);
    };

    Component(ComponentBase& parent, const Properties& properties);
    virtual ~Component() = default;

    template<typename Component, typename Properties>
    std::shared_ptr<Component> Emplace(const Properties& properties) {
        return std::static_pointer_cast<Component>(m_components.emplace_back(std::make_shared<Component>(dynamic_cast<ComponentBase&>(*this), properties)));
    }

    void Remove(const std::shared_ptr<ComponentBase> component);
    const std::string& Name() const;
    ComponentBase& Parent() const;
    ComponentBase& Root() const;
    bool IsRoot() const;

    bool IsEnabled() const;
    void Enable();
    void Disable();

    bool IsVisible() const;
    void Show();
    void Hide();

    static std::shared_ptr<ComponentBase> FocusedComponent();
    bool IsFocused() const;
    void GainFocus();
    void LoseFocus();
    static void RotateFocus();

    void BringToFront();
    void BringToBack();

    virtual void Render(sf::RenderWindow& window) = 0;
    virtual bool HandleEvent(const sf::Event& event) = 0;

    ComponentBase& operator[](std::string_view name);

protected:
    void Render_(sf::RenderWindow& window);
    bool HandleEvent_(const sf::Event& event);
    void Update_();
    void UpdateGeometry_();

private:
    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;
    Component(Component&&) = delete;
    Component& operator=(Component&&) = delete;

    virtual void OnRender(sf::RenderWindow& window) = 0;
    virtual void OnUpdate() = 0;
    virtual void OnShow() = 0;
    virtual void OnHide() = 0;
    virtual void OnEnable() = 0;
    virtual void OnDisable() = 0;
    virtual void OnGainFocus() = 0;
    virtual void OnLoseFocus() = 0;
    virtual void OnUpdateGeometry() = 0;

private:
    ComponentBase& m_parent;
    std::string m_name;
    bool m_enabled;
    bool m_visible;
    std::vector<std::shared_ptr<ComponentBase>> m_components;
};

}

#endif // SFUI_COMPONENT_H_INCLUDED
