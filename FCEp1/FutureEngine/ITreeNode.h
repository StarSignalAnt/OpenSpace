#pragma once
#include <string>
#include <vector>
#include <functional>

class Texture2D;

class ITreeNode
{
public:
    ITreeNode();
    ITreeNode(const std::string& text);
    ITreeNode(const std::string& text, Texture2D* icon);
    virtual ~ITreeNode();

    // Node properties
    void SetText(const std::string& text) { m_Text = text; }
    std::string GetText() const { return m_Text; }

    void SetIcon(Texture2D* icon) { m_Icon = icon; }
    Texture2D* GetIcon() const { return m_Icon; }

    void SetExpanded(bool expanded) { m_IsExpanded = expanded; }
    bool IsExpanded() const { return m_IsExpanded; }

    void SetSelected(bool selected) { m_IsSelected = selected; }
    bool IsSelected() const { return m_IsSelected; }

    // Child node management
    void AddChild(ITreeNode* child);
    void RemoveChild(ITreeNode* child);
    void ClearChildren();

    const std::vector<ITreeNode*>& GetChildren() const { return m_Children; }
    bool HasChildren() const { return !m_Children.empty(); }

    // Event handling
    void SetOnClick(std::function<void(ITreeNode*)> callback) { m_OnClick = callback; }
    void Click() { if (m_OnClick) m_OnClick(this); }

    // User data
    void SetUserData(void* data) { m_UserData = data; }
    void* GetUserData() const { return m_UserData; }

    // Parent node
    void SetParent(ITreeNode* parent) { m_Parent = parent; }
    ITreeNode* GetParent() const { return m_Parent; }

    // Depth calculation (useful for indentation)
    int GetDepth() const;
    void SetType(std::string type) {
        m_Type = type;
    }
    std::string GetType() {
        return m_Type;
    }

private:
    std::string m_Text;
    Texture2D* m_Icon;
    bool m_IsExpanded;
    bool m_IsSelected;
    std::vector<ITreeNode*> m_Children;
    ITreeNode* m_Parent;
    std::function<void(ITreeNode*)> m_OnClick;
    void* m_UserData;
    std::string m_Type = "UNKNOWN";
};