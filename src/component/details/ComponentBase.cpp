#include "component/details/ComponentBase.h"
#include "ComponentFactory.h"

using namespace sfui;

ComponentBase::ComponentBase(ComponentBase& parent, const Properties& properties)
    : ComponentHandlers(parent, properties)
{
}

ComponentBase::ComponentBase(ComponentBase& parent, ConfigView config)
    : ComponentHandlers(parent, config)
{
    for (const auto& componentConfig : config.array("elements")) {
        ComponentFactory::create(componentConfig, *this);
    }
}
