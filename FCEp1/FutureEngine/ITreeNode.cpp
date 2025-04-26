#include "ITreeNode.h"
#include <algorithm>

ITreeNode::ITreeNode()
    : m_Text(""),
    m_Icon(nullptr),
    m_IsExpanded(false),
    m_IsSelected(false),
    m_Parent(nullptr),
    m_UserData(nullptr)
{
}

ITreeNode::ITreeNode(const std::string& text)
    : m_Text(text),
    m_Icon(nullptr),
    m_IsExpanded(false),
    m_IsSelected(false),
    m_Parent(nullptr),
    m_UserData(nullptr)
{
}

ITreeNode::ITreeNode(const std::string& text, Texture2D* icon)
    : m_Text(text),
    m_Icon(icon),
    m_IsExpanded(false),
    m_IsSelected(false),
    m_Parent(nullptr),
    m_UserData(nullptr)
{
}

ITreeNode::~ITreeNode()
{
    // Delete all child nodes
    ClearChildren();
}

void ITreeNode::AddChild(ITreeNode* child)
{
    if (child)
    {
        child->SetParent(this);
        m_Children.push_back(child);
    }
}

void ITreeNode::RemoveChild(ITreeNode* child)
{
    if (child)
    {
        auto it = std::find(m_Children.begin(), m_Children.end(), child);
        if (it != m_Children.end())
        {
            (*it)->SetParent(nullptr);
            m_Children.erase(it);
        }
    }
}

void ITreeNode::ClearChildren()
{
    for (auto* child : m_Children)
    {
        delete child;
    }
    m_Children.clear();
}

int ITreeNode::GetDepth() const
{
    int depth = 0;
    ITreeNode* parent = m_Parent;

    while (parent)
    {
        depth++;
        parent = parent->GetParent();
    }

    return depth;
}