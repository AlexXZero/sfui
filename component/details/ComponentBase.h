#ifndef SFUI_COMPONENT_BASE_H_INCLUDED
#define SFUI_COMPONENT_BASE_H_INCLUDED

#include "Handlers.h"

namespace sfui {

class ComponentBase : public ComponentHandlers {
public:
    using Properties = ComponentHandlers::Properties;
    ComponentBase(ComponentBase& parent, const Properties& properties);
    ComponentBase(ComponentBase& parent, const nlohmann::json& json);
    virtual ~ComponentBase() = default;
};

}

#endif // SFUI_COMPONENT_BASE_H_INCLUDED
