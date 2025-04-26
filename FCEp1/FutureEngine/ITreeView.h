#pragma once
#include "IControl.h"
#include "ITreeNode.h"
#include "IVerticalScroller.h"
#include <vector>
#include <functional>

class Texture2D;

class ITreeView : public IControl
{
public:
    ITreeView();
    ITreeView(glm::vec2 position, glm::vec2 size);
    virtual ~ITreeView();

    // Tree management
    void AddRootNode(ITreeNode* node);
    void RemoveRootNode(ITreeNode* node);
    void ClearNodes();

    // Node expansion
    void ExpandNode(ITreeNode* node);
    void CollapseNode(ITreeNode* node);
    void ExpandAll();
    void CollapseAll();

    // Node selection
    void SelectNode(ITreeNode* node);
    ITreeNode* GetSelectedNode() const { return m_SelectedNode; }

    // Event callbacks
    void SetOnNodeSelected(std::function<void(ITreeNode*)> callback) { m_OnNodeSelected = callback; }
    void SetOnNodeExpanded(std::function<void(ITreeNode*)> callback) { m_OnNodeExpanded = callback; }
    void SetOnNodeCollapsed(std::function<void(ITreeNode*)> callback) { m_OnNodeCollapsed = callback; }

    // Drag and drop callbacks
    void SetOnDragStart(std::function<bool(ITreeNode*)> callback) { m_OnDragStart = callback; }
    void SetOnDragOver(std::function<bool(ITreeNode*, ITreeNode*)> callback) { m_OnDragOver = callback; }
    void SetOnDrop(std::function<void(ITreeNode*, ITreeNode*, bool)> callback) { m_OnDrop = callback; }

    // Enable/disable drag and drop
    void SetDragDropEnabled(bool enabled) { m_DragDropEnabled = enabled; }
    bool IsDragDropEnabled() const { return m_DragDropEnabled; }

    // Icons for expand/collapse indicators (optional)
    void SetExpandedIcon(Texture2D* icon) { m_ExpandedIcon = icon; }
    void SetCollapsedIcon(Texture2D* icon) { m_CollapsedIcon = icon; }

    // Appearance settings
    void SetNodeHeight(float height) { m_NodeHeight = height; }
    void SetIndentWidth(float width) { m_IndentWidth = width; }
    void SetNodeBackgroundColor(const glm::vec4& color) { m_NodeBackgroundColor = color; }
    void SetNodeSelectedColor(const glm::vec4& color) { m_NodeSelectedColor = color; }
    void SetNodeHoverColor(const glm::vec4& color) { m_NodeHoverColor = color; }
    void SetNodeTextColor(const glm::vec4& color) { m_NodeTextColor = color; }
    void SetDropTargetColor(const glm::vec4& color) { m_DropTargetColor = color; }
    void SetDragIndicatorColor(const glm::vec4& color) { m_DragIndicatorColor = color; }

    // Overridden methods from IControl
    void Update(float delta) override;
    void Render() override;
    void PreRender() override;
    void OnMouseDown(int button) override;
    void OnMouseUp(int button) override;
    void OnMouseMove(glm::vec2 position, glm::vec2 delta) override;
    void OnMouseWheel(float delta) override;
    void ExpandNodeRecursive(ITreeNode* node);
    void CollapseNodeRecursive(ITreeNode* node);
    
private:
    std::vector<ITreeNode*> m_RootNodes;
    ITreeNode* m_SelectedNode;
    ITreeNode* m_HoverNode;
    IVerticalScroller* m_Scroller;

    // Drag and drop state
    bool m_DragDropEnabled;
    ITreeNode* m_DraggedNode;
    ITreeNode* m_DropTargetNode;
    bool m_DragInProgress;
    glm::vec2 m_DragStartPos;
    bool m_DropAsChild;  // True if dropping as child, false if dropping as sibling

    // Appearance settings
    float m_NodeHeight;
    float m_IndentWidth;
    glm::vec4 m_NodeBackgroundColor;
    glm::vec4 m_NodeSelectedColor;
    glm::vec4 m_NodeHoverColor;
    glm::vec4 m_NodeTextColor;
    glm::vec4 m_DropTargetColor;
    glm::vec4 m_DragIndicatorColor;

    // Icons (optional)
    Texture2D* m_ExpandedIcon;
    Texture2D* m_CollapsedIcon;

    // Event callbacks
    std::function<void(ITreeNode*)> m_OnNodeSelected;
    std::function<void(ITreeNode*)> m_OnNodeExpanded;
    std::function<void(ITreeNode*)> m_OnNodeCollapsed;

    // Drag and drop callbacks
    std::function<bool(ITreeNode*)> m_OnDragStart;
    std::function<bool(ITreeNode*, ITreeNode*)> m_OnDragOver;
    std::function<void(ITreeNode*, ITreeNode*, bool)> m_OnDrop;

    // Internal helper methods
    void UpdateContentHeight();
    void RenderNode(ITreeNode* node, float& yOffset);
    ITreeNode* GetNodeAtPosition(glm::vec2 localPosition);
    ITreeNode* FindNodeAtPosition(const std::vector<ITreeNode*>& nodes, glm::vec2 localPosition, float& yOffset);
    void ToggleNodeExpansion(ITreeNode* node);
    bool IsNodeVisible(ITreeNode* node);

    // Helper to recursively calculate visible nodes
    int CountVisibleNodes();
    int CountVisibleNodesInBranch(ITreeNode* node, bool isRootExpanded = true);

    // Drag and drop helpers
    void StartDrag(ITreeNode* node);
    void UpdateDrag(glm::vec2 mousePos);
    void EndDrag(bool cancelled = false);
    bool CanDrop(ITreeNode* sourceNode, ITreeNode* targetNode, bool asChild);
    void PerformDrop();
    void RenderDragIndicator();
    void DetachNode(ITreeNode* node);
    void RemoveNodeFromParent(ITreeNode* node);
    bool FindNodeYPosition(ITreeNode* node, ITreeNode* targetNode, float& yOffset, float& result);
};