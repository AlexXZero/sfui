#ifndef SFUI_COMPONENT_H_INCLUDED
#define SFUI_COMPONENT_H_INCLUDED

#include "ComponentHandlers.h"
#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>

namespace sfui {

class Component : public ComponentHandlers {
public:
    Component(class ComponentContainer& parent, const nlohmann::json& json)
            : ComponentHandlers(parent, json) {}
    virtual ~Component() = default;

protected:
    virtual void Render(sf::RenderWindow& window) = 0;

private:
    friend class ComponentContainer;
    virtual void Render_(sf::RenderWindow& window) { Render(window); }
};

}

#endif // SFUI_COMPONENT_H_INCLUDED
