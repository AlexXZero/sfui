#include "ComponentBase.h"
#include "Parsers.h"

using namespace sfui;

ComponentBase::ComponentBase(ComponentBase& parent, const Properties& properties)
    : ComponentHandlers(parent, properties)
{
}

ComponentBase::ComponentBase(ComponentBase& parent, const nlohmann::json& json)
    : ComponentHandlers(parent, json)
{
    ParseComponents(*this, json);
}
