#ifndef SFUI_COMPONENT_BASE_H_INCLUDED
#define SFUI_COMPONENT_BASE_H_INCLUDED

#include "Handlers.h"

namespace sfui {

/**
 * @brief Composes all fundamental component aspects into a complete component.
 *
 * This class combines tree structure, lifecycle management, focus handling,
 * layout geometry, and event processing into a single component type.
 * It serves as the base for all concrete UI components in the system.
 *
 * @note Pure virtual methods Render() and HandleEvent() must be implemented
 *       by derived classes to provide component-specific behavior.
 */
class ComponentBase : public ComponentHandlers {
public:
    using Properties = ComponentHandlers::Properties;
    ComponentBase(ComponentBase& parent, const Properties& properties);
    ComponentBase(ComponentBase& parent, ConfigView config);
    virtual ~ComponentBase() = default;
};

}

#endif // SFUI_COMPONENT_BASE_H_INCLUDED
