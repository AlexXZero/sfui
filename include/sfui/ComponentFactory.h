#ifndef SFUI_COMPONENT_FACTORY_H_INCLUDED
#define SFUI_COMPONENT_FACTORY_H_INCLUDED

#include "ConfigParser.h"
#include "component/details/ComponentBase.h"
#include <string>

namespace sfui {

/**
 * @brief Factory responsible for runtime component creation from configuration objects.
 *
 * The factory maps string component type identifiers (for example "button"
 * or "panel") to component creation callbacks.
 *
 * Components are typically created while parsing UI configuration files.
 * Custom component types can be registered at runtime using registerComponent().
 *
 * Component requirements:
 * - Must derive from ComponentBase
 * - Must expose static constexpr ComponentTypeName
 * - Must be constructible from configuration data accepted by Emplace()
 *
 * The factory stores only static callbacks and does not own component instances.
 */
class ComponentFactory { // singleton: only static methods provided
public:
    /**
     * @brief Callback used to construct a component instance.
     *
     * @param config Component configuration object.
     * @param parent Parent component that will receive the new child component.
     */
    using ComponentCreator = void(*)(ConfigView, ComponentBase&);

public:
    /**
     * @brief Creates a component described by the specified configuration object.
     *
     * The configuration object must contain a valid string "type" field.
     * The corresponding registered component creator will be invoked and
     * the created component will be attached to the specified parent.
     *
     * @param config Component configuration object.
     * @param parent Parent component that will receive the created component.
     *
     * @throws std::runtime_error If the component type is missing or unsupported.
     */
    static void create(ConfigView config, ComponentBase& parent);

    /**
     * @brief Registers a component type in the factory.
     *
     * The component type name is taken from Component::ComponentTypeName.
     *
     * Registering the same component type more than once has no effect because
     * std::unordered_map::emplace() does not replace existing entries.
     *
     * @tparam Component Type of component to register.
     */
    template<typename Component>
    static void registerComponent() {
        registerComponent_(Component::ComponentTypeName, &componentCreator_<Component>);
    }

private:
    /**
     * @brief Registers a component creator callback.
     *
     * @param key String component type identifier.
     * @param parser Callback responsible for component creation.
     */
    static void registerComponent_(std::string key, ComponentCreator parser);

    /**
     * @brief Internal helper used to construct a specific component type.
     *
     * @tparam Component Component type to construct.
     *
     * @param config Component configuration object.
     * @param parent Parent component that will receive the created child.
     */
    template<typename Component>
    static void componentCreator_(ConfigView config, ComponentBase& parent) {
        parent.Emplace<Component>(config);
    }

private:
    /**
     * @brief Registry of component creation callbacks indexed by component type.
     */
    static std::unordered_map<std::string, ComponentCreator> g_componentCreators;
};

}

#endif // SFUI_COMPONENT_FACTORY_H_INCLUDED
