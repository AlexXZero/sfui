#ifndef SFUI_COMPONENT_NODE_H_INCLUDED
#define SFUI_COMPONENT_NODE_H_INCLUDED

#include <nlohmann/json.hpp>
#include <string>

namespace sfui {

class ComponentContainer;

class ComponentNode {
public:
    ComponentNode(ComponentContainer& parent, const nlohmann::json& json);
    virtual ~ComponentNode() = default;

    const std::string& Name() const;
    ComponentContainer& Parent() const;
    ComponentContainer& Root() const;
    bool IsRoot() const;

private:
    ComponentNode(const ComponentNode&) = delete;
    ComponentNode& operator=(const ComponentNode&) = delete;
    ComponentNode(ComponentNode&&) = delete;
    ComponentNode& operator=(ComponentNode&&) = delete;

private:
    ComponentContainer& m_parent;
    std::string m_name;
};

}

#endif // SFUI_COMPONENT_NODE_H_INCLUDED
