#include "ComponentNode.h"
#include "ComponentContainer.h"

using namespace sfui;

ComponentNode::ComponentNode(ComponentContainer& parent, const nlohmann::json& json)
        : m_parent(parent), m_name(json["name"]) {}

const std::string& ComponentNode::Name() const
{
    return m_name;
}

ComponentContainer& ComponentNode::Parent() const
{
    return m_parent;
}

ComponentContainer& ComponentNode::Root() const
{
    return IsRoot() ? m_parent : m_parent.Root();
}

bool ComponentNode::IsRoot() const
{
    return this == &m_parent;
}
