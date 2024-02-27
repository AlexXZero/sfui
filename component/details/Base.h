#ifndef SFUI_COMPONENT_BASE_H_INCLUDED
#define SFUI_COMPONENT_BASE_H_INCLUDED

#include "Handlers.h"
#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>

namespace sfui {

class Component : public ComponentHandlers {
public:
    Component(Component& parent, const nlohmann::json& json)
            : ComponentHandlers(parent, json) {}
    virtual ~Component() = default;
    virtual void Render(sf::RenderWindow& window) = 0;

private:
    friend class ComponentContainer;
    virtual void Render_(sf::RenderWindow& window) { Render(window); HandleRender(); }
};

using ComponentBase = Component;

}

#endif // SFUI_COMPONENT_BASE_H_INCLUDED
