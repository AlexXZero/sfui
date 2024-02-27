#ifndef SFUI_COMPONENT_NODE_H_INCLUDED
#define SFUI_COMPONENT_NODE_H_INCLUDED

#include <nlohmann/json.hpp>
#include <string>

namespace sfui {

class Component;

class ComponentNode {
public:
    ComponentNode(Component& parent, const nlohmann::json& json);
    virtual ~ComponentNode() = default;

    const std::string& Name() const;
    Component& Parent() const;
    Component& Root() const;
    bool IsRoot() const;

private:
    ComponentNode(const ComponentNode&) = delete;
    ComponentNode& operator=(const ComponentNode&) = delete;
    ComponentNode(ComponentNode&&) = delete;
    ComponentNode& operator=(ComponentNode&&) = delete;

private:
    Component& m_parent;
    std::string m_name;
};

}

#endif // SFUI_COMPONENT_NODE_H_INCLUDED
