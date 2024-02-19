#ifndef SFUI_COMPONENT_CONTAINER_H_INCLUDED
#define SFUI_COMPONENT_CONTAINER_H_INCLUDED

#include "Component.h"
#include <nlohmann/json.hpp>
#include <vector>
#include <memory>
#include <string_view>

namespace sfui {

class ComponentContainer: public Component {
public:
    ComponentContainer(ComponentContainer& parent, const nlohmann::json& json);
    virtual ~ComponentContainer() = default;

    template<typename Component, typename... Args>
    Component& Emplace(const Args&... args) {
        return static_cast<Component&>(*m_components.emplace_back(std::make_unique<Component>(*this, args...)));
    }

    // Move the child component to the front (top) of the rendering and event handling order
    void BringToFront(const Component& component);
    void BringToBack(const Component& component);

    Component& operator[](std::string_view name);

private:
    friend class Window;
    virtual void Render_(sf::RenderWindow& window) override;
    virtual bool HandleEvent_(const sf::Event& event) override;

private:
    std::vector<std::unique_ptr<Component>> m_components;
};

}

#endif // SFUI_COMPONENT_CONTAINER_H_INCLUDED
