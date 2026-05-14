#include "ComponentFactory.h"
#include "component/Form.h"
#include "component/Edit.h"
#include "component/Panel.h"
#include "component/Image.h"
#include "component/Label.h"
#include "component/Button.h"
#include <unordered_map>

using namespace sfui;

std::unordered_map<std::string, ComponentFactory::ComponentCreator> ComponentFactory::g_componentCreators = {
    { std::string(Button::ComponentTypeName), &componentCreator_<Button> },
    { std::string(Image::ComponentTypeName),  &componentCreator_<Image>  },
    { std::string(Label::ComponentTypeName),  &componentCreator_<Label>  },
    { std::string(Panel::ComponentTypeName),  &componentCreator_<Panel>  },
    { std::string(Edit::ComponentTypeName),   &componentCreator_<Edit>   },
    { std::string(Form::ComponentTypeName),   &componentCreator_<Form>   },
};

void ComponentFactory::create(ConfigView config, ComponentBase& parent)
{
    auto componentType = config.required<std::string>("type");
    if (auto it = g_componentCreators.find(componentType); it != g_componentCreators.end()) {
        it->second(config, parent);
    } else {
        throw std::runtime_error("\"" + componentType + "\" type component not found");
    }
}

void ComponentFactory::registerComponent_(std::string key, ComponentCreator parser)
{
    if (!g_componentCreators.emplace(key, parser).second) {
        throw std::runtime_error("Component already registered: " + key);
    }
}
