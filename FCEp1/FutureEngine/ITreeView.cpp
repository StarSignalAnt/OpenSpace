#include "ITreeView.h"
#include "UIHelp.h"
#include "GameInput.h"
#include "Texture2D.h"
#include <algorithm>
#include <iostream>
#include "FutureApp.h"

ITreeView::ITreeView()
    : IControl(),
    m_SelectedNode(nullptr),
    m_HoverNode(nullptr),
    m_DragDropEnabled(true),
    m_DraggedNode(nullptr),
    m_DropTargetNode(nullptr),
    m_DragInProgress(false),
    m_DropAsChild(false),
    m_NodeHeight(22.0f),
    m_IndentWidth(20.0f),
    m_NodeBackgroundColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)),
    m_NodeSelectedColor(glm::vec4(0.4f, 0.6f, 0.8f, 1.0f)),
    m_NodeHoverColor(glm::vec4(0.3f, 0.5f, 0.7f, 1.0f)),
    m_NodeTextColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
    m_DropTargetColor(glm::vec4(0.2f, 0.8f, 0.2f, 1.0f)),
    m_DragIndicatorColor(glm::vec4(0.0f, 0.7f, 1.0f, 1.0f)),
    m_ExpandedIcon(nullptr),
    m_CollapsedIcon(nullptr)
{
    // Create the vertical scroller
    m_Scroller = new IVerticalScroller(glm::vec2(m_Size.x - 15.0f, 0.0f), glm::vec2(15.0f, m_Size.y));
    AddChild(m_Scroller);

    // Setup scroller callback
    m_Scroller->SetOnScrolled([this](float offset) {
        // This will be called whenever the scroll position changes
        });

    m_CullChildren = true;
}

ITreeView::ITreeView(glm::vec2 position, glm::vec2 size)
    : IControl(position, size),
    m_SelectedNode(nullptr),
    m_HoverNode(nullptr),
    m_DragDropEnabled(true),
    m_DraggedNode(nullptr),
    m_DropTargetNode(nullptr),
    m_DragInProgress(false),
    m_DropAsChild(false),
    m_NodeHeight(22.0f),
    m_IndentWidth(20.0f),
    m_NodeBackgroundColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)),
    m_NodeSelectedColor(glm::vec4(0.4f, 0.6f, 0.8f, 1.0f)),
    m_NodeHoverColor(glm::vec4(0.3f, 0.5f, 0.7f, 1.0f)),
    m_NodeTextColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)),
    m_DropTargetColor(glm::vec4(0.2f, 0.8f, 0.2f, 1.0f)),
    m_DragIndicatorColor(glm::vec4(0.0f, 0.7f, 1.0f, 1.0f)),
    m_ExpandedIcon(nullptr),
    m_CollapsedIcon(nullptr)
{
    // Create the vertical scroller
    m_Scroller = new IVerticalScroller(glm::vec2(size.x - 15.0f, 0.0f), glm::vec2(15.0f, size.y));
    AddChild(m_Scroller);

    // Setup scroller callback
    m_Scroller->SetOnScrolled([this](float offset) {
        // This will be called whenever the scroll position changes
        });

    m_CullChildren = true;
}

ITreeView::~ITreeView()
{
    ClearNodes();
    // m_Scroller is deleted by IControl's destructor since it's a child
}

void ITreeView::AddRootNode(ITreeNode* node)
{
    if (node)
    {
        m_RootNodes.push_back(node);
        UpdateContentHeight();
    }
}

void ITreeView::RemoveRootNode(ITreeNode* node)
{
    if (node)
    {
        auto it = std::find(m_RootNodes.begin(), m_RootNodes.end(), node);
        if (it != m_RootNodes.end())
        {
            if (*it == m_SelectedNode)
            {
                m_SelectedNode = nullptr;
            }

            if (*it == m_HoverNode)
            {
                m_HoverNode = nullptr;
            }

            m_RootNodes.erase(it);
            UpdateContentHeight();
        }
    }
}

void ITreeView::ClearNodes()
{
    for (auto* node : m_RootNodes)
    {
        delete node;
    }
    m_RootNodes.clear();
    m_SelectedNode = nullptr;
    m_HoverNode = nullptr;
    UpdateContentHeight();
}

void ITreeView::ExpandNode(ITreeNode* node)
{
    if (node && !node->IsExpanded())
    {
        node->SetExpanded(true);

        if (m_OnNodeExpanded)
        {
            m_OnNodeExpanded(node);
        }

        UpdateContentHeight();
    }
}

void ITreeView::CollapseNode(ITreeNode* node)
{
    if (node && node->IsExpanded())
    {
        node->SetExpanded(false);

        if (m_OnNodeCollapsed)
        {
            m_OnNodeCollapsed(node);
        }

        UpdateContentHeight();
    }
}

void ITreeView::ExpandAll()
{
    for (auto* node : m_RootNodes)
    {
        ExpandNodeRecursive(node);
    }

    UpdateContentHeight();
}

void ITreeView::ExpandNodeRecursive(ITreeNode* node)
{
    if (node)
    {
        node->SetExpanded(true);

        for (auto* child : node->GetChildren())
        {
            ExpandNodeRecursive(child);
        }
    }
}

void ITreeView::CollapseAll()
{
    for (auto* node : m_RootNodes)
    {
        CollapseNodeRecursive(node);
    }

    UpdateContentHeight();
}

void ITreeView::CollapseNodeRecursive(ITreeNode* node)
{
    if (node)
    {
        node->SetExpanded(false);

        for (auto* child : node->GetChildren())
        {
            CollapseNodeRecursive(child);
        }
    }
}

void ITreeView::SelectNode(ITreeNode* node)
{
    if (m_SelectedNode != node)
    {
        if (m_SelectedNode)
        {
            m_SelectedNode->SetSelected(false);
        }

        m_SelectedNode = node;

        if (m_SelectedNode)
        {
            m_SelectedNode->SetSelected(true);

            if (m_OnNodeSelected)
            {
                m_OnNodeSelected(m_SelectedNode);
            }
        }
    }
}

void ITreeView::Update(float delta)
{
    // Update the scroller
    m_Scroller->Update(delta);

    // Update the position of the scroller
    m_Scroller->Set(glm::vec2(m_Size.x - 15.0f, 0.0f), glm::vec2(15.0f, m_Size.y));

    // Update other children
    UpdateChildren(delta);
}

void ITreeView::Render()
{
    glm::vec2 renderPos = GetRenderPosition();

    // Draw background
    UIHelp::DrawRect(renderPos, m_Size, m_NodeBackgroundColor);

    // Draw border
    UIHelp::DrawOutlineRect(renderPos, m_Size, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

    // Enable scissor test to prevent nodes from rendering outside the TreeView
    int windowHeight = FutureApp::m_Inst->GetHeight();
    UIHelp::setScissor(renderPos.x, renderPos.y, m_Size.x - 15.0f, m_Size.y, windowHeight);

    // Calculate the vertical offset based on scroll position
    float yOffset = -m_Scroller->GetContentOffset();

    // Render all visible nodes
    for (auto* node : m_RootNodes)
    {
        RenderNode(node, yOffset);
    }

    // Render drag indicator if we're dragging
    if (m_DragInProgress && m_DropTargetNode)
    {
        RenderDragIndicator();
    }

    // Disable scissor test
    UIHelp::RemoveScissor();

    // Render the scroller and other children
    RenderChildren();

    // Render the drag node at cursor position if dragging
    if (m_DragInProgress && m_DraggedNode)
    {
        // Render the dragged node at the cursor position
        glm::vec2 mousePos = GameInput::MousePosition;
        glm::vec2 nodeSize = glm::vec2(m_Size.x / 2, m_NodeHeight);
        glm::vec2 nodePos = mousePos - glm::vec2(nodeSize.x / 2, m_NodeHeight / 2);

        // Draw semi-transparent background
        UIHelp::DrawRect(nodePos, nodeSize, glm::vec4(m_NodeSelectedColor.r, m_NodeSelectedColor.g, m_NodeSelectedColor.b, 0.55f));

        // Draw node text
        float textOffset = 4.0f;
        if (m_DraggedNode->GetIcon())
        {
            glm::vec2 iconPos = nodePos + glm::vec2(textOffset, 2.0f);
            glm::vec2 iconSize = glm::vec2(16.0f, 16.0f);
            UIHelp::DrawImage(iconPos, iconSize, m_DraggedNode->GetIcon(), glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
            textOffset += 20.0f;
        }

        UIHelp::DrawText(nodePos + glm::vec2(textOffset, (m_NodeHeight - UIHelp::StrHeight(m_DraggedNode->GetText())) / 2.0f),
            m_DraggedNode->GetText(), glm::vec4(m_NodeTextColor.r, m_NodeTextColor.g, m_NodeTextColor.b, 0.7f));
    }
}

void ITreeView::PreRender()
{
    IControl::PreRender();
}

void ITreeView::OnMouseDown(int button)
{
    if (button == 0) // Left mouse button
    {
        glm::vec2 mousePos = GameInput::MousePosition;
        glm::vec2 renderPos = GetRenderPosition();
        glm::vec2 localPos = mousePos - renderPos;

        // Check if click is in the content area (not on scroller)
        if (localPos.x < m_Size.x - 15.0f)
        {
            ITreeNode* clickedNode = GetNodeAtPosition(localPos);

            if (clickedNode)
            {
                // Store the initial mouse position for potential drag operation
                m_DragStartPos = mousePos;

                // Check if clicked on the expand/collapse icon
                float nodeIndent = clickedNode->GetDepth() * m_IndentWidth;
                if (clickedNode->HasChildren() && localPos.x >= nodeIndent && localPos.x <= nodeIndent + 16.0f)
                {
                    // Toggle expansion
                    ToggleNodeExpansion(clickedNode);
                }
                else
                {
                    // Select node
                    SelectNode(clickedNode);

                    // Trigger node click event
                    clickedNode->Click();
                }
            }
            else
            {
                // Clicked on empty space, deselect current node
                SelectNode(nullptr);
            }
        }
    }
}

void ITreeView::OnMouseUp(int button)
{
    if (button == 0 && m_DragInProgress) // Left mouse button
    {
        if (m_DraggedNode && m_DropTargetNode)
        {
            PerformDrop();
        }

        EndDrag();
    }
}

void ITreeView::OnMouseMove(glm::vec2 position, glm::vec2 delta)
{
    glm::vec2 renderPos = GetRenderPosition();
    glm::vec2 localPos = GameInput::MousePosition - renderPos;

    // Handle dragging
    if (GameInput::Buttons[MOUSE_BUTTON_LEFT] && m_DragDropEnabled)
    {
        if (!m_DragInProgress && m_SelectedNode)
        {
            // Check if we've moved enough to start a drag
            glm::vec2 dragDiff = GameInput::MousePosition - m_DragStartPos;
            float dragDistance = sqrt(dragDiff.x * dragDiff.x + dragDiff.y * dragDiff.y);

            if (dragDistance > 5.0f) // Drag threshold
            {
                StartDrag(m_SelectedNode);
            }
        }
        else if (m_DragInProgress)
        {
            UpdateDrag(GameInput::MousePosition);
        }
    }

    // Update hover state
    if (localPos.x < m_Size.x - 15.0f)
    {
        ITreeNode* nodeUnderMouse = GetNodeAtPosition(localPos);

        if (m_HoverNode != nodeUnderMouse)
        {
            m_HoverNode = nodeUnderMouse;
        }
    }
    else
    {
        if (m_HoverNode)
        {
            m_HoverNode = nullptr;
        }
    }
}

void ITreeView::OnMouseWheel(float delta)
{
    // Forward mouse wheel to scroller
    m_Scroller->OnMouseWheel(delta);
}

void ITreeView::UpdateContentHeight()
{
    // Calculate total height of all visible nodes
    float contentHeight = CountVisibleNodes() * m_NodeHeight;

    // Update scroller content height
    m_Scroller->SetContentHeight(contentHeight);

    // Fix scroll position if needed
    m_Scroller->Fix();
}

void ITreeView::RenderNode(ITreeNode* node, float& yOffset)
{
    if (!node) return;

    glm::vec2 renderPos = GetRenderPosition();

    // Check if node is in the visible area
    if (yOffset + m_NodeHeight >= 0 && yOffset <= m_Size.y)
    {
        float nodeDepth = node->GetDepth();
        float indentation = nodeDepth * m_IndentWidth;
        glm::vec2 nodePos = renderPos + glm::vec2(indentation, yOffset);
        glm::vec2 nodeSize = glm::vec2(m_Size.x - 15.0f - indentation, m_NodeHeight);

        // Draw node background based on state
        if (node == m_DraggedNode)
        {
            // Don't draw background for dragged node
        }
        else if (node == m_DropTargetNode)
        {
            UIHelp::DrawRect(nodePos, nodeSize, m_DropTargetColor);
        }
        else if (node == m_SelectedNode)
        {
            UIHelp::DrawRect(nodePos, nodeSize, m_NodeSelectedColor);
        }
        else if (node == m_HoverNode)
        {
            UIHelp::DrawRect(nodePos, nodeSize, m_NodeHoverColor);
        }

        // Draw expand/collapse indicator if node has children
        if (node->HasChildren())
        {
            glm::vec2 iconPos = nodePos - glm::vec2(16.0f, -3.0f);
            glm::vec2 iconSize = glm::vec2(16.0f, 16.0f);

            if (node->IsExpanded())
            {
                if (m_ExpandedIcon)
                {
                    UIHelp::DrawImage(iconPos, iconSize, m_ExpandedIcon, glm::vec4(1.0f));
                }
                else
                {
                    // Draw a simple minus icon
                    UIHelp::DrawRect(iconPos + glm::vec2(4.0f, 7.0f), glm::vec2(8.0f, 2.0f), m_NodeTextColor);
                }
            }
            else
            {
                if (m_CollapsedIcon)
                {
                    UIHelp::DrawImage(iconPos, iconSize, m_CollapsedIcon, glm::vec4(1.0f));
                }
                else
                {
                    // Draw a simple plus icon
                    UIHelp::DrawRect(iconPos + glm::vec2(4.0f, 7.0f), glm::vec2(8.0f, 2.0f), m_NodeTextColor);
                    UIHelp::DrawRect(iconPos + glm::vec2(7.0f, 4.0f), glm::vec2(2.0f, 8.0f), m_NodeTextColor);
                }
            }
        }

        // Skip rendering the dragged node
        if (node != m_DraggedNode || !m_DragInProgress)
        {
            // Draw node icon if it has one (now optional)
            float textOffset = 4.0f;
            if (node->GetIcon())
            {
                glm::vec2 iconPos = nodePos + glm::vec2(textOffset, 2.0f);
                glm::vec2 iconSize = glm::vec2(16.0f, 16.0f);
                UIHelp::DrawImage(iconPos, iconSize, node->GetIcon(), glm::vec4(1.0f));
                textOffset += 20.0f;
            }

            // Draw node text
            UIHelp::DrawText(nodePos + glm::vec2(textOffset, (m_NodeHeight - UIHelp::StrHeight(node->GetText())) / 2.0f),
                node->GetText(), m_NodeTextColor);
        }
    }

    // Move to the next node position
    yOffset += m_NodeHeight;

    // Render children if expanded
    if (node->IsExpanded())
    {
        for (auto* child : node->GetChildren())
        {
            RenderNode(child, yOffset);
        }
    }
}

ITreeNode* ITreeView::GetNodeAtPosition(glm::vec2 localPosition)
{
    // Adjust for scrolling
    float yOffset = -m_Scroller->GetContentOffset();

    // Search all root nodes
    return FindNodeAtPosition(m_RootNodes, localPosition, yOffset);
}

ITreeNode* ITreeView::FindNodeAtPosition(const std::vector<ITreeNode*>& nodes, glm::vec2 localPosition, float& yOffset)
{
    for (auto* node : nodes)
    {
        // Check if the click is within this node's vertical bounds
        if (localPosition.y >= yOffset && localPosition.y < yOffset + m_NodeHeight)
        {
            // Check horizontal bounds (taking indentation into account)
            float nodeDepth = node->GetDepth();
            float indentation = nodeDepth * m_IndentWidth;

            if (localPosition.x >= indentation && localPosition.x < m_Size.x - 15.0f)
            {
                return node;
            }
        }

        // Move to next node position
        yOffset += m_NodeHeight;

        // Check children if expanded
        if (node->IsExpanded())
        {
            ITreeNode* childResult = FindNodeAtPosition(node->GetChildren(), localPosition, yOffset);
            if (childResult)
            {
                return childResult;
            }
        }
    }

    return nullptr;
}

void ITreeView::ToggleNodeExpansion(ITreeNode* node)
{
    if (node)
    {
        if (node->IsExpanded())
        {
            CollapseNode(node);
        }
        else
        {
            ExpandNode(node);
        }
    }
}

bool ITreeView::IsNodeVisible(ITreeNode* node)
{
    if (!node) return false;

    // Check if all parent nodes are expanded
    ITreeNode* parent = node->GetParent();
    while (parent)
    {
        if (!parent->IsExpanded())
        {
            return false;
        }
        parent = parent->GetParent();
    }

    return true;
}

int ITreeView::CountVisibleNodes()
{
    int count = 0;

    for (auto* node : m_RootNodes)
    {
        count++; // Count root node

        if (node->IsExpanded())
        {
            count += CountVisibleNodesInBranch(node);
        }
    }

    return count;
}

int ITreeView::CountVisibleNodesInBranch(ITreeNode* node, bool isRootExpanded)
{
    if (!node) return 0;

    int count = 0;

    // Only count children if parent is expanded
    if (isRootExpanded)
    {
        for (auto* child : node->GetChildren())
        {
            count++; // Count this child

            // Count grandchildren if this child is expanded
            if (child->IsExpanded())
            {
                count += CountVisibleNodesInBranch(child);
            }
        }
    }

    return count;
}



// Drag and drop implementation
void ITreeView::StartDrag(ITreeNode* node)
{
    if (!node || !m_DragDropEnabled)
        return;

    // Check if dragging is allowed for this node
    if (m_OnDragStart && !m_OnDragStart(node))
        return;

    m_DraggedNode = node;
    m_DragInProgress = true;
    m_DropTargetNode = nullptr;
    m_DropAsChild = false;
}

void ITreeView::UpdateDrag(glm::vec2 mousePos)
{
    if (!m_DragInProgress || !m_DraggedNode)
        return;

    glm::vec2 renderPos = GetRenderPosition();
    glm::vec2 localPos = mousePos - renderPos;

    // Auto-scroll when dragging near edges
    float scrollSpeed = 0.0f;
    if (localPos.y < 20.0f)
    {
        // Scroll up
        scrollSpeed = -5.0f * (20.0f - localPos.y) / 20.0f;
    }
    else if (localPos.y > m_Size.y - 20.0f)
    {
        // Scroll down
        scrollSpeed = 5.0f * (localPos.y - (m_Size.y - 20.0f)) / 20.0f;
    }

    if (scrollSpeed != 0.0f)
    {
        float newPos = m_Scroller->GetScrollPosition() + scrollSpeed / m_Scroller->GetContentHeight();
        m_Scroller->SetScrollPosition(newPos);
    }

    // Find the potential drop target
    ITreeNode* nodeUnderMouse = GetNodeAtPosition(localPos);

    // Determine if we're dropping as a child or sibling
    if (nodeUnderMouse)
    {
        // Get node position info
        float yOffset = -m_Scroller->GetContentOffset();
        float nodeY = 0;

        // Find the Y position of the node
        for (auto* rootNode : m_RootNodes)
        {
            float startY = yOffset;
            FindNodeYPosition(rootNode, nodeUnderMouse, yOffset, nodeY);
            if (nodeY != 0) break;
            yOffset = startY;
        }

        // Calculate the node's middle Y position
        float nodeMiddleY = nodeY + m_NodeHeight / 2;

        // If mouse is in the upper third of the node, drop as sibling above
        // If mouse is in the lower third of the node, drop as sibling below
        // If mouse is in the middle third of the node, drop as child
        float upperThird = nodeY + m_NodeHeight / 3;
        float lowerThird = nodeY + (m_NodeHeight * 2) / 3;

        if (localPos.y < upperThird)
        {
            // Drop as sibling above
            m_DropAsChild = false;
            m_DropTargetNode = nodeUnderMouse;
        }
        else if (localPos.y > lowerThird)
        {
            // Drop as sibling below
            m_DropAsChild = false;
            m_DropTargetNode = nodeUnderMouse;
        }
        else
        {
            // Drop as child
            m_DropAsChild = true;
            m_DropTargetNode = nodeUnderMouse;
        }

        // Check if the drop is allowed
        if (!CanDrop(m_DraggedNode, m_DropTargetNode, m_DropAsChild))
        {
            m_DropTargetNode = nullptr;
        }
    }
    else
    {
        m_DropTargetNode = nullptr;
    }
}

void ITreeView::EndDrag(bool cancelled)
{
    m_DragInProgress = false;
    m_DraggedNode = nullptr;
    m_DropTargetNode = nullptr;
}

bool ITreeView::CanDrop(ITreeNode* sourceNode, ITreeNode* targetNode, bool asChild)
{
    if (!sourceNode || !targetNode)
        return false;

    // Can't drop a node onto itself
    if (sourceNode == targetNode)
        return false;

    // Can't drop a parent onto one of its children (would create a cycle)
    ITreeNode* parent = targetNode->GetParent();
    while (parent)
    {
        if (parent == sourceNode)
            return false;
        parent = parent->GetParent();
    }

    // Check user-defined drag-over callback
    if (m_OnDragOver)
    {
        return m_OnDragOver(sourceNode, targetNode);
    }

    return true;
}

void ITreeView::PerformDrop()
{
    if (!m_DraggedNode || !m_DropTargetNode)
        return;

    // Remove the node from its current parent
    RemoveNodeFromParent(m_DraggedNode);

    if (m_DropAsChild)
    {
        // Add as a child of the target node
        m_DropTargetNode->AddChild(m_DraggedNode);

        // Expand the target node to show the newly added child
        if (!m_DropTargetNode->IsExpanded())
        {
            ExpandNode(m_DropTargetNode);
        }
    }
    else
    {
        // Add as a sibling of the target node
        if (ITreeNode* targetParent = m_DropTargetNode->GetParent())
        {
            // Get the index of the target node in its parent's children
            auto& siblings = targetParent->GetChildren();
            auto it = std::find(siblings.begin(), siblings.end(), m_DropTargetNode);
            if (it != siblings.end())
            {
                size_t index = std::distance(siblings.begin(), it);

                // Insert at this index
                targetParent->AddChild(m_DraggedNode);

                // TODO: Implement proper insertion at index if needed
                // This would require modifying the ITreeNode class to support insertion at index
            }
            else
            {
                // Fallback: just add as child
                targetParent->AddChild(m_DraggedNode);
            }
        }
        else
        {
            // Target is a root node, add to root nodes
            auto it = std::find(m_RootNodes.begin(), m_RootNodes.end(), m_DropTargetNode);
            if (it != m_RootNodes.end())
            {
                size_t index = std::distance(m_RootNodes.begin(), it);
                m_RootNodes.insert(m_RootNodes.begin() + index, m_DraggedNode);
            }
            else
            {
                // Fallback: just add as a root node
                m_RootNodes.push_back(m_DraggedNode);
            }
        }
    }

    // Call the drop callback
    if (m_OnDrop)
    {
        m_OnDrop(m_DraggedNode, m_DropTargetNode, m_DropAsChild);
    }

    // Update the tree view
    UpdateContentHeight();
}

void ITreeView::RenderDragIndicator()
{
    if (!m_DropTargetNode || !m_DragInProgress)
        return;

    glm::vec2 renderPos = GetRenderPosition();

    // Find the Y position of the drop target node
    float yOffset = -m_Scroller->GetContentOffset();
    float nodeY = 0;

    // Find the Y position of the node
    for (auto* rootNode : m_RootNodes)
    {
        float startY = yOffset;
        FindNodeYPosition(rootNode, m_DropTargetNode, yOffset, nodeY);
        if (nodeY != 0) break;
        yOffset = startY;
    }

    if (nodeY == 0)
        return;

    float nodeDepth = m_DropTargetNode->GetDepth();
    float indentation = nodeDepth * m_IndentWidth;

    // Determine indicator position based on drop type
    if (m_DropAsChild)
    {
        // Draw indicator showing it will be added as a child
        glm::vec2 indicatorPos = renderPos + glm::vec2(indentation + 10.0f, nodeY + m_NodeHeight);
        glm::vec2 indicatorSize = glm::vec2(m_Size.x - 25.0f - indentation - 10.0f, 2.0f);
        UIHelp::DrawRect(indicatorPos, indicatorSize, m_DragIndicatorColor);

        // Draw small vertical line to indicate child relationship
        glm::vec2 vertLinePos = renderPos + glm::vec2(indentation + 10.0f, nodeY + m_NodeHeight - 3.0f);
        glm::vec2 vertLineSize = glm::vec2(2.0f, 6.0f);
        UIHelp::DrawRect(vertLinePos, vertLineSize, m_DragIndicatorColor);
    }
    else
    {
        // Draw indicator showing it will be added as a sibling
        glm::vec2 indicatorPos = renderPos + glm::vec2(indentation, nodeY);
        glm::vec2 indicatorSize = glm::vec2(m_Size.x - 25.0f - indentation, 2.0f);
        UIHelp::DrawRect(indicatorPos, indicatorSize, m_DragIndicatorColor);
    }
}

void ITreeView::RemoveNodeFromParent(ITreeNode* node)
{
    if (!node)
        return;

    // If the node has a parent, remove it from the parent's children
    if (ITreeNode* parent = node->GetParent())
    {
        parent->RemoveChild(node);
    }
    else
    {
        // If it's a root node, remove it from the root nodes
        auto it = std::find(m_RootNodes.begin(), m_RootNodes.end(), node);
        if (it != m_RootNodes.end())
        {
            m_RootNodes.erase(it);
        }
    }

    // Make sure the node doesn't have a parent reference
    node->SetParent(nullptr);
}

bool ITreeView::FindNodeYPosition(ITreeNode* node, ITreeNode* targetNode, float& yOffset, float& result)
{
    if (!node)
        return false;

    if (node == targetNode)
    {
        result = yOffset;
        return true;
    }

    // Move to the next node position
    yOffset += m_NodeHeight;

    // Check children if expanded
    if (node->IsExpanded())
    {
        for (auto* child : node->GetChildren())
        {
            if (FindNodeYPosition(child, targetNode, yOffset, result))
                return true;
        }
    }

    return false;
}