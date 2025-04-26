#include "ITextArea.h"
#include "UIHelp.h"
#include "GameInput.h"
#include "FutureApp.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cctype>
#include <sstream>

ITextArea::ITextArea() : IControl() {
    m_VerticalScroller = new IVerticalScroller();
    m_HorizontalScroller = new IHorizontalScroller();

    // Configure scrollers
    m_VerticalScroller->SetOnScrolled([this](float offset) {
        m_VerticalScrollOffset = offset;
        });

    m_HorizontalScroller->SetOnScrolled([this](float offset) {
        m_HorizontalScrollOffset = offset;
        });

    // Add scrollers as child controls
    AddChild(m_VerticalScroller);
    AddChild(m_HorizontalScroller);
}

ITextArea::ITextArea(glm::vec2 position, glm::vec2 size) : IControl(position, size) {
    m_VerticalScroller = new IVerticalScroller(
        glm::vec2(size.x - 10.0f, 0.0f),
        glm::vec2(10.0f, size.y - 10.0f)
    );

    m_HorizontalScroller = new IHorizontalScroller(
        glm::vec2(0.0f, size.y - 10.0f),
        glm::vec2(size.x - 10.0f, 10.0f)
    );

    // Configure scrollers
    m_VerticalScroller->SetOnScrolled([this](float offset) {
        m_VerticalScrollOffset = offset;
        });

    m_HorizontalScroller->SetOnScrolled([this](float offset) {
        m_HorizontalScrollOffset = offset;
        });

    // Add scrollers as child controls
    AddChild(m_VerticalScroller);
    AddChild(m_HorizontalScroller);

    // Set initial empty text
    SetText("");
}

void ITextArea::SetText(const std::string& text) {
    m_Text = text;
    SplitTextIntoLines();
    UpdateContentDimensions();

    if (OnTextChanged) {
        OnTextChanged(m_Text);
    }
}

std::string ITextArea::GetText() const {
    return m_Text;
}

void ITextArea::SplitTextIntoLines() {
    m_Lines.clear();
    std::stringstream ss(m_Text);
    std::string line;

    while (std::getline(ss, line, '\n')) {
        m_Lines.push_back(line);
    }

    // If text ends with newline, add empty line
    if (!m_Text.empty() && m_Text.back() == '\n') {
        m_Lines.push_back("");
    }

    // If there are no lines, add at least one empty line
    if (m_Lines.empty()) {
        m_Lines.push_back("");
    }

    // Ensure cursor is in valid position
    m_CursorLine = std::min(m_CursorLine, static_cast<int>(m_Lines.size()) - 1);
    m_CursorColumn = std::min(m_CursorColumn, static_cast<int>(m_Lines[m_CursorLine].length()));
}

void ITextArea::UpdateContentDimensions() {
    m_MaxContentWidth = 0.0f;

    // Calculate max width of all lines
    for (const auto& line : m_Lines) {
        float lineWidth = GetLineWidth(line);
        m_MaxContentWidth = std::max(m_MaxContentWidth, lineWidth);
    }

    // Add padding
    m_MaxContentWidth += m_Padding * 2;

    // Calculate total height
    m_MaxContentHeight = m_Lines.size() * m_LineHeight + m_Padding * 2;

    // Update scrollers
    m_VerticalScroller->SetContentHeight(m_MaxContentHeight);

    if (m_AllowHorizontalScroll) {
        m_HorizontalScroller->SetContentWidth(m_MaxContentWidth);
    }
    else {
        m_HorizontalScroller->SetContentWidth(m_Size.x - 10.0f);
    }
}

float ITextArea::GetLineWidth(const std::string& line) const {
    return UIHelp::StrWidth(line);
}

void ITextArea::Update(float delta) {
    // Position scrollbars
    m_VerticalScroller->Set(
        glm::vec2(m_Size.x - 10.0f, 0.0f),
        glm::vec2(10.0f, m_Size.y - 10.0f)
    );

    m_HorizontalScroller->Set(
        glm::vec2(0.0f, m_Size.y - 10.0f),
        glm::vec2(m_Size.x - 10.0f, 10.0f)
    );

    // Update cursor blink
    if (m_HasFocus) {
        m_CursorBlinkTime += delta;
        if (m_CursorBlinkTime >= 0.5f) {
            m_CursorBlinkTime = 0.0f;
            m_CursorVisible = !m_CursorVisible;
        }

        // Update key repeat timers
        for (auto& keyState : m_KeyStates) {
            if (keyState.second.isPressed) {
                keyState.second.timePressed += delta;
                keyState.second.timeSinceRepeat += delta;
            }
        }

        // Process keyboard input
        HandleKeyInput();
    }

    // Update scrollers
    UpdateChildren(delta);
}

void ITextArea::PreRender() {
    PreRenderChildren();
}

void ITextArea::Render() {
    glm::vec2 renderPos = GetRenderPosition();

    // Draw background
    UIHelp::DrawRect(renderPos, m_Size, m_BackgroundColor);

    // Draw border
    UIHelp::DrawOutlineRect(renderPos, m_Size, m_BorderColor);

    // Set up scissor for text area (excluding scrollbars)
    int sh = FutureApp::m_Inst->GetHeight();
    UIHelp::setScissor(
        renderPos.x + m_Padding,
        renderPos.y + m_Padding,
        m_Size.x - 10.0f - m_Padding * 2,
        m_Size.y - 10.0f - m_Padding * 2,
        sh
    );
    //glEnable(GL_SCISSOR_TEST);


    // Draw content
    DrawContent();

    // Draw selection highlights
    DrawSelectionHighlights();

    // Draw cursor if focused and visible
    if (m_HasFocus && m_CursorVisible) {
        DrawCursor();
    }


    //glDisable(GL_SCISSOR_TEST);
    UIHelp::RemoveScissor();

    // Render scrollbars and other children
    RenderChildren();
}

void ITextArea::DrawContent() {
    glm::vec2 renderPos = GetRenderPosition();
    float startY = renderPos.y + m_Padding - m_VerticalScrollOffset;

    // Determine visible line range
    int firstVisibleLine = std::max(0, static_cast<int>(m_VerticalScrollOffset / m_LineHeight));
    int lastVisibleLine = std::min(
        static_cast<int>(m_Lines.size()) - 1,
        static_cast<int>((m_VerticalScrollOffset + m_Size.y - 10.0f) / m_LineHeight)
    );

    // Draw visible lines
    for (int i = firstVisibleLine; i <= lastVisibleLine; ++i) {
        float lineY = startY + i * m_LineHeight;

        // Skip if line is outside visible area
        if (lineY + m_LineHeight < renderPos.y || lineY > renderPos.y + m_Size.y - 10.0f) {
            continue;
        }

        // Draw line text
        UIHelp::DrawText(
            glm::vec2(renderPos.x + m_Padding - m_HorizontalScrollOffset, lineY),
            m_Lines[i],
            m_TextColor
        );
    }
}

void ITextArea::DrawSelectionHighlights() {
    // Only draw if there's a valid selection
    if (m_SelectionStartLine < 0 || m_SelectionEndLine < 0) {
        return;
    }

    glm::vec2 renderPos = GetRenderPosition();
    float startY = renderPos.y + m_Padding - m_VerticalScrollOffset;

    // Ensure selection has start before end
    int startLine = m_SelectionStartLine;
    int startCol = m_SelectionStartColumn;
    int endLine = m_SelectionEndLine;
    int endCol = m_SelectionEndColumn;

    if (startLine > endLine || (startLine == endLine && startCol > endCol)) {
        std::swap(startLine, endLine);
        std::swap(startCol, endCol);
    }

    // Determine visible line range
    int firstVisibleLine = std::max(0, static_cast<int>(m_VerticalScrollOffset / m_LineHeight));
    int lastVisibleLine = std::min(
        static_cast<int>(m_Lines.size()) - 1,
        static_cast<int>((m_VerticalScrollOffset + m_Size.y - 10.0f) / m_LineHeight)
    );

    // Only process visible lines that have selection
    int selFirstLine = std::max(firstVisibleLine, startLine);
    int selLastLine = std::min(lastVisibleLine, endLine);

    for (int i = selFirstLine; i <= selLastLine; ++i) {
        float lineY = startY + i * m_LineHeight;

        // Calculate selection start/end for this line
        int lineStartCol = (i == startLine) ? startCol : 0;
        int lineEndCol = (i == endLine) ? endCol : m_Lines[i].length();

        if (lineStartCol >= lineEndCol) {
            continue;
        }

        // Calculate positions
        float startX = UIHelp::StrWidth(m_Lines[i].substr(0, lineStartCol));
        float endX = UIHelp::StrWidth(m_Lines[i].substr(0, lineEndCol));

        // Draw selection rectangle
        UIHelp::DrawRect(
            glm::vec2(renderPos.x + m_Padding + startX - m_HorizontalScrollOffset, lineY),
            glm::vec2(endX - startX, m_LineHeight),
            m_SelectionColor
        );
    }
}

void ITextArea::DrawCursor() {
    glm::vec2 renderPos = GetRenderPosition();
    float startY = renderPos.y + m_Padding - m_VerticalScrollOffset;

    // Calculate cursor position
    float cursorY = startY + m_CursorLine * m_LineHeight;

    // Only draw if cursor line is visible
    if (cursorY < renderPos.y || cursorY > renderPos.y + m_Size.y - 10.0f) {
        return;
    }

    std::string linePrefix = "";
    if (m_CursorColumn > 0 && m_CursorLine < m_Lines.size()) {
        linePrefix = m_Lines[m_CursorLine].substr(0, m_CursorColumn);
    }

    float cursorX = UIHelp::StrWidth(linePrefix);

    // Draw cursor line
    UIHelp::DrawRect(
        glm::vec2(renderPos.x + m_Padding + cursorX - m_HorizontalScrollOffset, cursorY),
        glm::vec2(1.0f, m_LineHeight),
        m_CursorColor
    );
}

void ITextArea::OnMouseDown(int button) {
    if (button == MOUSE_BUTTON_LEFT) {
        m_HasFocus = true;

        glm::vec2 mousePos = GameInput::MousePosition;
        glm::vec2 renderPos = GetRenderPosition();

        // Ignore if click is on scrollbars
        if (mousePos.x > renderPos.x + m_Size.x - 10.0f || mousePos.y > renderPos.y + m_Size.y - 10.0f) {
            return;
        }

        // Calculate line index based on click position
        float relY = mousePos.y - (renderPos.y + m_Padding) + m_VerticalScrollOffset;
        int lineIndex = static_cast<int>(relY / m_LineHeight);
        lineIndex = std::max(0, std::min(lineIndex, static_cast<int>(m_Lines.size()) - 1));

        // Calculate column index
        float relX = mousePos.x - (renderPos.x + m_Padding) + m_HorizontalScrollOffset;
        int columnIndex = GetCharacterIndexAtPosition(lineIndex, glm::vec2(relX, 0));

        // Update cursor position
        m_CursorLine = lineIndex;
        m_CursorColumn = columnIndex;

        // Start selection
        m_SelectionStartLine = m_CursorLine;
        m_SelectionStartColumn = m_CursorColumn;
        m_SelectionEndLine = -1;
        m_SelectionEndColumn = -1;

        m_IsDragging = true;

        // Reset cursor blink
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
    }
}

void ITextArea::OnMouseUp(int button) {
    if (button == MOUSE_BUTTON_LEFT) {
        m_IsDragging = false;

        // If no real selection was made, clear selection
        if (m_SelectionStartLine == m_SelectionEndLine &&
            m_SelectionStartColumn == m_SelectionEndColumn) {
            ClearSelection();
        }
    }
}

void ITextArea::OnMouseMove(glm::vec2 position, glm::vec2 delta) {
    if (!m_IsDragging) {
        return;
    }

    // Update selection end position
    glm::vec2 mousePos = GameInput::MousePosition;
    glm::vec2 renderPos = GetRenderPosition();

    // Calculate line index
    float relY = mousePos.y - (renderPos.y + m_Padding) + m_VerticalScrollOffset;
    int lineIndex = static_cast<int>(relY / m_LineHeight);
    lineIndex = std::max(0, std::min(lineIndex, static_cast<int>(m_Lines.size()) - 1));

    // Calculate column index
    float relX = mousePos.x - (renderPos.x + m_Padding) + m_HorizontalScrollOffset;
    int columnIndex = GetCharacterIndexAtPosition(lineIndex, glm::vec2(relX, 0));

    // Update cursor and selection
    m_CursorLine = lineIndex;
    m_CursorColumn = columnIndex;
    m_SelectionEndLine = lineIndex;
    m_SelectionEndColumn = columnIndex;

    // Ensure cursor is visible
    EnsureCursorVisible();

    // Reset cursor blink
    m_CursorBlinkTime = 0.0f;
    m_CursorVisible = true;
}

int ITextArea::GetCharacterIndexAtPosition(int lineIndex, glm::vec2 position) const {
    if (lineIndex < 0 || lineIndex >= m_Lines.size()) {
        return 0;
    }

    const std::string& line = m_Lines[lineIndex];
    float x = position.x;

    // Handle empty line
    if (line.empty()) {
        return 0;
    }

    // Find the character index closest to the x position
    for (int i = 0; i <= line.length(); i++) {
        float charWidth = 0;
        if (i < line.length()) {
            std::string charStr(1, line[i]);
            charWidth = UIHelp::StrWidth(charStr);
        }

        float charPos = UIHelp::StrWidth(line.substr(0, i));

        if (x <= charPos + charWidth / 2) {
            return i;
        }
    }

    return line.length();
}

void ITextArea::ClearSelection() {
    m_SelectionStartLine = -1;
    m_SelectionStartColumn = -1;
    m_SelectionEndLine = -1;
    m_SelectionEndColumn = -1;
}

std::string ITextArea::GetSelectedText() const {
    if (m_SelectionStartLine < 0 || m_SelectionEndLine < 0) {
        return "";
    }

    // Ensure selection has start before end
    int startLine = m_SelectionStartLine;
    int startCol = m_SelectionStartColumn;
    int endLine = m_SelectionEndLine;
    int endCol = m_SelectionEndColumn;

    if (startLine > endLine || (startLine == endLine && startCol > endCol)) {
        std::swap(startLine, endLine);
        std::swap(startCol, endCol);
    }

    // Single line selection
    if (startLine == endLine) {
        return m_Lines[startLine].substr(startCol, endCol - startCol);
    }

    // Multi-line selection
    std::string result = m_Lines[startLine].substr(startCol) + "\n";

    for (int i = startLine + 1; i < endLine; i++) {
        result += m_Lines[i] + "\n";
    }

    result += m_Lines[endLine].substr(0, endCol);

    return result;
}

bool ITextArea::HandleKeyWithRepeat(int key) {
    // Check if the key is currently pressed
    if (GameInput::Keys[key]) {
        // If this is the first time the key is pressed, initialize its state
        if (!m_KeyStates[key].isPressed) {
            m_KeyStates[key].isPressed = true;
            m_KeyStates[key].timePressed = 0.0f;
            m_KeyStates[key].timeSinceRepeat = 0.0f;
            m_KeyStates[key].hasRepeated = false;

            return true; // First press is always registered
        }
        else {
            // Key is already pressed, check if it should repeat
            if (!m_KeyStates[key].hasRepeated) {
                // Check if initial delay has passed
                if (m_KeyStates[key].timePressed >= m_KeyRepeatDelay) {
                    m_KeyStates[key].hasRepeated = true;
                    m_KeyStates[key].timeSinceRepeat = 0.0f;
                    return true; // First repeat after delay
                }
            }
            else {
                // Already in repeat mode, check interval
                if (m_KeyStates[key].timeSinceRepeat >= m_KeyRepeatInterval) {
                    m_KeyStates[key].timeSinceRepeat = 0.0f;
                    return true; // Regular repeat at interval
                }
            }
        }
    }
    else {
        // Key is released, reset its state
        if (m_KeyStates.find(key) != m_KeyStates.end() && m_KeyStates[key].isPressed) {
            m_KeyStates[key].isPressed = false;
        }
    }

    return false; // Key press not registered at this time
}

void ITextArea::HandleKeyInput() {
    if (m_IsReadOnly) return;

    bool ctrl = GameInput::Keys[GLFW_KEY_LEFT_CONTROL] || GameInput::Keys[GLFW_KEY_RIGHT_CONTROL];
    bool shift = GameInput::Keys[GLFW_KEY_LEFT_SHIFT] || GameInput::Keys[GLFW_KEY_RIGHT_SHIFT];

    // Handle navigation keys
    if (HandleKeyWithRepeat(GLFW_KEY_LEFT)) {
        if (ctrl) {
            // Move to previous word
            if (m_CursorColumn > 0) {
                const std::string& line = m_Lines[m_CursorLine];
                int pos = m_CursorColumn - 1;

                // Skip non-alphanumeric characters
                while (pos > 0 && !isalnum(line[pos - 1])) pos--;

                // Skip word
                while (pos > 0 && isalnum(line[pos - 1])) pos--;

                m_CursorColumn = pos;
            }
            else if (m_CursorLine > 0) {
                // Move to end of previous line
                m_CursorLine--;
                m_CursorColumn = m_Lines[m_CursorLine].length();
            }
        }
        else {
            // Move left one character
            if (m_CursorColumn > 0) {
                m_CursorColumn--;
            }
            else if (m_CursorLine > 0) {
                // Move to end of previous line
                m_CursorLine--;
                m_CursorColumn = m_Lines[m_CursorLine].length();
            }
        }

        // Update selection if shift is pressed
        if (shift) {
            if (m_SelectionStartLine == -1) {
                m_SelectionStartLine = m_CursorLine;
                m_SelectionStartColumn = m_CursorColumn + 1;
            }
            m_SelectionEndLine = m_CursorLine;
            m_SelectionEndColumn = m_CursorColumn;
        }
        else {
            ClearSelection();
        }

        EnsureCursorVisible();
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
    }

    if (HandleKeyWithRepeat(GLFW_KEY_RIGHT)) {
        if (ctrl) {
            // Move to next word
            const std::string& line = m_Lines[m_CursorLine];
            if (m_CursorColumn < line.length()) {
                int pos = m_CursorColumn;

                // Skip current word
                while (pos < line.length() && isalnum(line[pos])) pos++;

                // Skip non-alphanumeric characters
                while (pos < line.length() && !isalnum(line[pos])) pos++;

                m_CursorColumn = pos;
            }
            else if (m_CursorLine < m_Lines.size() - 1) {
                // Move to start of next line
                m_CursorLine++;
                m_CursorColumn = 0;
            }
        }
        else {
            // Move right one character
            if (m_CursorColumn < m_Lines[m_CursorLine].length()) {
                m_CursorColumn++;
            }
            else if (m_CursorLine < m_Lines.size() - 1) {
                // Move to start of next line
                m_CursorLine++;
                m_CursorColumn = 0;
            }
        }

        // Update selection if shift is pressed
        if (shift) {
            if (m_SelectionStartLine == -1) {
                m_SelectionStartLine = m_CursorLine;
                m_SelectionStartColumn = m_CursorColumn - 1;
            }
            m_SelectionEndLine = m_CursorLine;
            m_SelectionEndColumn = m_CursorColumn;
        }
        else {
            ClearSelection();
        }

        EnsureCursorVisible();
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
    }

    if (HandleKeyWithRepeat(GLFW_KEY_UP)) {
        if (m_CursorLine > 0) {
            m_CursorLine--;

            // Adjust column to not exceed line length
            m_CursorColumn = std::min(m_CursorColumn, static_cast<int>(m_Lines[m_CursorLine].length()));
        }

        // Update selection if shift is pressed
        if (shift) {
            if (m_SelectionStartLine == -1) {
                m_SelectionStartLine = m_CursorLine + 1;
                m_SelectionStartColumn = m_CursorColumn;
            }
            m_SelectionEndLine = m_CursorLine;
            m_SelectionEndColumn = m_CursorColumn;
        }
        else {
            ClearSelection();
        }

        EnsureCursorVisible();
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
    }

    if (HandleKeyWithRepeat(GLFW_KEY_DOWN)) {
        if (m_CursorLine < m_Lines.size() - 1) {
            m_CursorLine++;

            // Adjust column to not exceed line length
            m_CursorColumn = std::min(m_CursorColumn, static_cast<int>(m_Lines[m_CursorLine].length()));
        }

        // Update selection if shift is pressed
        if (shift) {
            if (m_SelectionStartLine == -1) {
                m_SelectionStartLine = m_CursorLine - 1;
                m_SelectionStartColumn = m_CursorColumn;
            }
            m_SelectionEndLine = m_CursorLine;
            m_SelectionEndColumn = m_CursorColumn;
        }
        else {
            ClearSelection();
        }

        EnsureCursorVisible();
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
    }

    if (HandleKeyWithRepeat(GLFW_KEY_HOME)) {
        if (ctrl) {
            // Go to start of document
            m_CursorLine = 0;
            m_CursorColumn = 0;
        }
        else {
            // Go to start of line
            m_CursorColumn = 0;
        }

        // Update selection if shift is pressed
        if (shift) {
            if (m_SelectionStartLine == -1) {
                m_SelectionStartLine = m_CursorLine;
                m_SelectionStartColumn = m_CursorColumn;
            }
            m_SelectionEndLine = m_CursorLine;
            m_SelectionEndColumn = m_CursorColumn;
        }
        else {
            ClearSelection();
        }

        EnsureCursorVisible();
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
    }

    if (HandleKeyWithRepeat(GLFW_KEY_END)) {
        if (ctrl) {
            // Go to end of document
            m_CursorLine = m_Lines.size() - 1;
            m_CursorColumn = m_Lines[m_CursorLine].length();
        }
        else {
            // Go to end of line
            m_CursorColumn = m_Lines[m_CursorLine].length();
        }

        // Update selection if shift is pressed
        if (shift) {
            if (m_SelectionStartLine == -1) {
                m_SelectionStartLine = m_CursorLine;
                m_SelectionStartColumn = m_CursorColumn;
            }
            m_SelectionEndLine = m_CursorLine;
            m_SelectionEndColumn = m_CursorColumn;
        }
        else {
            ClearSelection();
        }

        EnsureCursorVisible();
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
    }

    if (HandleKeyWithRepeat(GLFW_KEY_PAGE_UP)) {
        // Move cursor up by number of visible lines
        int visibleLines = static_cast<int>((m_Size.y - 10.0f - m_Padding * 2) / m_LineHeight);
        m_CursorLine = std::max(0, m_CursorLine - visibleLines);

        // Adjust column to not exceed line length
        m_CursorColumn = std::min(m_CursorColumn, static_cast<int>(m_Lines[m_CursorLine].length()));

        // Update selection if shift is pressed
        if (shift) {
            if (m_SelectionStartLine == -1) {
                m_SelectionStartLine = m_CursorLine + visibleLines;
                m_SelectionStartColumn = m_CursorColumn;
            }
            m_SelectionEndLine = m_CursorLine;
            m_SelectionEndColumn = m_CursorColumn;
        }
        else {
            ClearSelection();
        }

        EnsureCursorVisible();
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
    }

    if (HandleKeyWithRepeat(GLFW_KEY_PAGE_DOWN)) {
        // Move cursor down by number of visible lines
        int visibleLines = static_cast<int>((m_Size.y - 10.0f - m_Padding * 2) / m_LineHeight);
        m_CursorLine = std::min(static_cast<int>(m_Lines.size()) - 1, m_CursorLine + visibleLines);

        // Adjust column to not exceed line length
        m_CursorColumn = std::min(m_CursorColumn, static_cast<int>(m_Lines[m_CursorLine].length()));

        // Update selection if shift is pressed
        if (shift) {
            if (m_SelectionStartLine == -1) {
                m_SelectionStartLine = m_CursorLine - visibleLines;
                m_SelectionStartColumn = m_CursorColumn;
            }
            m_SelectionEndLine = m_CursorLine;
            m_SelectionEndColumn = m_CursorColumn;
        }
        else {
            ClearSelection();
        }

        EnsureCursorVisible();
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
    }

    // Handle text editing keys
    if (HandleKeyWithRepeat(GLFW_KEY_BACKSPACE)) {
        if (m_SelectionStartLine != -1 && m_SelectionEndLine != -1) {
            // Delete selection
            DeleteSelectedText();
        }
        else if (m_CursorColumn > 0) {
            // Delete character before cursor
            std::string& line = m_Lines[m_CursorLine];
            line.erase(m_CursorColumn - 1, 1);
            m_CursorColumn--;

            // Rebuild text and update dimensions
            RebuildText();
        }
        else if (m_CursorLine > 0) {
            // Merge with previous line
            int prevLineLength = m_Lines[m_CursorLine - 1].length();
            m_Lines[m_CursorLine - 1] += m_Lines[m_CursorLine];
            m_Lines.erase(m_Lines.begin() + m_CursorLine);

            m_CursorLine--;
            m_CursorColumn = prevLineLength;

            // Rebuild text and update dimensions
            RebuildText();
        }

        EnsureCursorVisible();
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
    }

    if (HandleKeyWithRepeat(GLFW_KEY_DELETE)) {
        if (m_SelectionStartLine != -1 && m_SelectionEndLine != -1) {
            // Delete selection
            DeleteSelectedText();
        }
        else if (m_CursorColumn < m_Lines[m_CursorLine].length()) {
            // Delete character after cursor
            std::string& line = m_Lines[m_CursorLine];
            line.erase(m_CursorColumn, 1);

            // Rebuild text and update dimensions
            RebuildText();
        }
        else if (m_CursorLine < m_Lines.size() - 1) {
            // Merge with next line
            m_Lines[m_CursorLine] += m_Lines[m_CursorLine + 1];
            m_Lines.erase(m_Lines.begin() + m_CursorLine + 1);

            // Rebuild text and update dimensions
            RebuildText();
        }

        EnsureCursorVisible();
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
    }

    // Handle Enter key
    if (HandleKeyWithRepeat(GLFW_KEY_ENTER)) {
        if (m_SelectionStartLine != -1 && m_SelectionEndLine != -1) {
            // Delete selection first
            DeleteSelectedText();
        }

        // Split current line
        std::string& currentLine = m_Lines[m_CursorLine];
        std::string newLine = currentLine.substr(m_CursorColumn);
        currentLine.erase(m_CursorColumn);

        // Insert new line
        m_Lines.insert(m_Lines.begin() + m_CursorLine + 1, newLine);

        // Update cursor position
        m_CursorLine++;
        m_CursorColumn = 0;

        // Rebuild text and update dimensions
        RebuildText();

        EnsureCursorVisible();
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
    }

    // Handle Tab key
    if (HandleKeyWithRepeat(GLFW_KEY_TAB)) {
        if (m_SelectionStartLine != -1 && m_SelectionEndLine != -1) {
            // Delete selection first
            DeleteSelectedText();
        }

        // Insert tab (4 spaces)
        std::string& line = m_Lines[m_CursorLine];
        line.insert(m_CursorColumn, "    ");
        m_CursorColumn += 4;

        // Rebuild text and update dimensions
        RebuildText();

        EnsureCursorVisible();
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
    }

    // Handle Ctrl+A (select all)
    if (ctrl && GameInput::Keys[GLFW_KEY_A]) {
        GameInput::Keys[GLFW_KEY_A] = false; // Consume the key press

        m_SelectionStartLine = 0;
        m_SelectionStartColumn = 0;
        m_SelectionEndLine = m_Lines.size() - 1;
        m_SelectionEndColumn = m_Lines.back().length();

        m_CursorLine = m_SelectionEndLine;
        m_CursorColumn = m_SelectionEndColumn;

        EnsureCursorVisible();
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
    }

    // Handle character input
    for (int key = 32; key <= 126; key++) { // ASCII printable characters
        if (HandleKeyWithRepeat(key)) {
            if (ctrl) continue; // Skip if ctrl is held

            if (m_SelectionStartLine != -1 && m_SelectionEndLine != -1) {
                // Delete selection first
                DeleteSelectedText();
            }

            // Handle character input
            char c = (char)key;

            // Handle letter case properly
            if (key >= 'A' && key <= 'Z') {
                // For letter keys, use lowercase by default
                if (!shift) {
                    c = (char)(key + 32); // Convert to lowercase
                }
            }
            else if (shift) {
                // Map number keys to symbols
                switch (key) {
                case '1': c = '!'; break;
                case '2': c = '@'; break;
                case '3': c = '#'; break;
                case '4': c = '$'; break;
                case '5': c = '%'; break;
                case '6': c = '^'; break;
                case '7': c = '&'; break;
                case '8': c = '*'; break;
                case '9': c = '('; break;
                case '0': c = ')'; break;
                case '-': c = '_'; break;
                case '=': c = '+'; break;
                case '[': c = '{'; break;
                case ']': c = '}'; break;
                case '\\': c = '|'; break;
                case ';': c = ':'; break;
                case '\'': c = '"'; break;
                case ',': c = '<'; break;
                case '.': c = '>'; break;
                case '/': c = '?'; break;
                case '`': c = '~'; break;
                }
            }

            // Insert character
            ProcessTextInput(c);

            EnsureCursorVisible();
            m_CursorBlinkTime = 0.0f;
            m_CursorVisible = true;
        }
    }
}

void ITextArea::ProcessTextInput(char c) {
    // Insert character at cursor position
    std::string& line = m_Lines[m_CursorLine];
    line.insert(m_CursorColumn, 1, c);
    m_CursorColumn++;

    // Rebuild text and update dimensions
    RebuildText();
}

void ITextArea::RebuildText() {
    // Rebuild text from lines
    m_Text.clear();

    for (size_t i = 0; i < m_Lines.size(); i++) {
        m_Text += m_Lines[i];
        if (i < m_Lines.size() - 1) {
            m_Text += "\n";
        }
    }

    UpdateContentDimensions();

    if (OnTextChanged) {
        OnTextChanged(m_Text);
    }
}

void ITextArea::DeleteSelectedText() {
    if (m_SelectionStartLine == -1 || m_SelectionEndLine == -1) {
        return;
    }

    // Ensure selection has start before end
    int startLine = m_SelectionStartLine;
    int startCol = m_SelectionStartColumn;
    int endLine = m_SelectionEndLine;
    int endCol = m_SelectionEndColumn;

    if (startLine > endLine || (startLine == endLine && startCol > endCol)) {
        std::swap(startLine, endLine);
        std::swap(startCol, endCol);
    }

    // Single line selection
    if (startLine == endLine) {
        m_Lines[startLine].erase(startCol, endCol - startCol);
    }
    else {
        // Multi-line selection

        // Modify first line
        m_Lines[startLine].erase(startCol);

        // Modify last line
        std::string endLineRemainder = m_Lines[endLine].substr(endCol);

        // Append remainder to first line
        m_Lines[startLine] += endLineRemainder;

        // Remove lines in between
        m_Lines.erase(m_Lines.begin() + startLine + 1, m_Lines.begin() + endLine + 1);
    }

    // Set cursor position to start of selection
    m_CursorLine = startLine;
    m_CursorColumn = startCol;

    // Clear selection
    ClearSelection();

    // Rebuild text and update dimensions
    RebuildText();
}

void ITextArea::EnsureCursorVisible() {
    // Vertical scrolling
    float cursorY = m_CursorLine * m_LineHeight;
    float viewportHeight = m_Size.y - 10.0f - m_Padding * 2;

    if (cursorY < m_VerticalScrollOffset) {
        // Cursor is above visible area
        m_VerticalScrollOffset = cursorY;
        m_VerticalScroller->SetScrollPosition(m_VerticalScrollOffset / (m_MaxContentHeight - viewportHeight));
    }
    else if (cursorY + m_LineHeight > m_VerticalScrollOffset + viewportHeight) {
        // Cursor is below visible area
        m_VerticalScrollOffset = cursorY + m_LineHeight - viewportHeight;
        m_VerticalScroller->SetScrollPosition(m_VerticalScrollOffset / (m_MaxContentHeight - viewportHeight));
    }

    // Horizontal scrolling
    if (m_AllowHorizontalScroll) {
        std::string linePrefix = "";
        if (m_CursorColumn > 0 && m_CursorLine < m_Lines.size()) {
            linePrefix = m_Lines[m_CursorLine].substr(0, m_CursorColumn);
        }

        float cursorX = UIHelp::StrWidth(linePrefix);
        float viewportWidth = m_Size.x - 10.0f - m_Padding * 2;

        if (cursorX < m_HorizontalScrollOffset) {
            // Cursor is to the left of visible area
            m_HorizontalScrollOffset = cursorX;
            m_HorizontalScroller->SetScrollPosition(m_HorizontalScrollOffset / (m_MaxContentWidth - viewportWidth));
        }
        else if (cursorX > m_HorizontalScrollOffset + viewportWidth - 10) {
            // Cursor is to the right of visible area (with a small margin)
            m_HorizontalScrollOffset = cursorX - viewportWidth + 10;
            m_HorizontalScroller->SetScrollPosition(m_HorizontalScrollOffset / (m_MaxContentWidth - viewportWidth));
        }
    }
}

void ITextArea::SetCursorPosition(int line, int column) {
    // Clamp line and column to valid ranges
    line = std::max(0, std::min(line, static_cast<int>(m_Lines.size()) - 1));
    column = std::max(0, std::min(column, static_cast<int>(m_Lines[line].length())));

    m_CursorLine = line;
    m_CursorColumn = column;

    // Clear selection
    ClearSelection();

    // Make cursor visible
    EnsureCursorVisible();
    m_CursorBlinkTime = 0.0f;
    m_CursorVisible = true;
}

std::pair<int, int> ITextArea::GetCursorPosition() const {
    return std::make_pair(m_CursorLine, m_CursorColumn);
}

void ITextArea::SelectText(int startLine, int startCol, int endLine, int endCol) {
    // Clamp values to valid ranges
    startLine = std::max(0, std::min(startLine, static_cast<int>(m_Lines.size()) - 1));
    startCol = std::max(0, std::min(startCol, static_cast<int>(m_Lines[startLine].length())));

    endLine = std::max(0, std::min(endLine, static_cast<int>(m_Lines.size()) - 1));
    endCol = std::max(0, std::min(endCol, static_cast<int>(m_Lines[endLine].length())));

    // Set selection
    m_SelectionStartLine = startLine;
    m_SelectionStartColumn = startCol;
    m_SelectionEndLine = endLine;
    m_SelectionEndColumn = endCol;

    // Set cursor to end of selection
    m_CursorLine = endLine;
    m_CursorColumn = endCol;

    // Ensure cursor is visible
    EnsureCursorVisible();
}

void ITextArea::ScrollToPosition(float verticalPos, float horizontalPos) {
    // Set scroll positions (0.0 to 1.0)
    m_VerticalScroller->SetScrollPosition(verticalPos);
    m_HorizontalScroller->SetScrollPosition(horizontalPos);
}

void ITextArea::ScrollToCursor() {
    EnsureCursorVisible();
}

void ITextArea::SetFocus(bool focus) {
    m_HasFocus = focus;

    if (focus) {
        // Reset cursor blink when gaining focus
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
    }
}

void ITextArea::OnActivate() {
    SetFocus(true);
}

void ITextArea::OnDeactivate() {
    SetFocus(false);
}