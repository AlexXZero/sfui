#include "Node.h"
#include "Container.h"

using namespace sfui;

ComponentNode::ComponentNode(Component& parent, const nlohmann::json& json)
        : m_parent(parent), m_name(json["name"]) {}

const std::string& ComponentNode::Name() const
{
    return m_name;
}

Component& ComponentNode::Parent() const
{
    return m_parent;
}

Component& ComponentNode::Root() const
{
    return IsRoot() ? m_parent : m_parent.Root();
}

bool ComponentNode::IsRoot() const
{
    return this == &m_parent;
}
