#include "ITextEdit.h"
#include "UIHelp.h"
#include "GameInput.h"
#include "FutureApp.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cctype>
#include "GameUI.h"

ITextEdit::ITextEdit() : IControl() {
    m_CursorPosition = 0;
    m_SelectionStart = -1;
    m_SelectionEnd = -1;
    m_ScrollOffset = 0.0f;
    if (m_IsNumeric) {
        m_Text = "0";
        m_CursorPosition = 1;
    }
    SetIgnoreScissor(false);
}

ITextEdit::ITextEdit(glm::vec2 position, glm::vec2 size) : IControl(position, size) {
    m_CursorPosition = 0;
    m_SelectionStart = -1;
    m_SelectionEnd = -1;
    m_ScrollOffset = 0.0f;
    if (m_IsNumeric) {
        m_Text = "0";
        m_CursorPosition = 1;
    }
    SetIgnoreScissor(false);
}


void ITextEdit::Render() {
    glm::vec2 renderPos = GetRenderPosition();

    Edited();

    // Draw background
    UIHelp::DrawRect(renderPos, m_Size, m_BackgroundColor);

    // Draw border
    UIHelp::DrawOutlineRect(renderPos, m_Size+glm::vec2(-1,-1), m_BorderColor);

    // Calculate visible text
    std::string displayText = m_Text;
    if (m_IsPassword) {
        displayText = std::string(m_Text.length(), '*');
    }

    // Calculate text dimensions and positioning
    float textHeight = UIHelp::StrHeight(displayText);
    float textY = renderPos.y + (m_Size.y - textHeight) / 2.0f;

    // Calculate visible portion of text based on scroll offset
    float visibleWidth = m_Size.x - 10.0f; // Padding (5px on each side)

    // Draw selection if any
    if (m_HasFocus && m_SelectionStart >= 0 && m_SelectionEnd >= 0 && m_SelectionStart != m_SelectionEnd) {
        int start = std::min(m_SelectionStart, m_SelectionEnd);
        int end = std::max(m_SelectionStart, m_SelectionEnd);

        float startX = UIHelp::StrWidth(displayText.substr(0, start)) - m_ScrollOffset;
        float endX = UIHelp::StrWidth(displayText.substr(0, end)) - m_ScrollOffset;

        // Clamp to visible area
        startX = std::max(0.0f, startX);
        endX = std::min(visibleWidth, endX);

        if (endX > startX) {
            UIHelp::DrawRect(
                renderPos + glm::vec2(5.0f + startX, 2.0f),
                glm::vec2(endX - startX, m_Size.y - 4.0f),
                m_SelectionColor
            );
        }
    }

    // Set up scissor rect for text clipping
    int sh = FutureApp::m_Inst->GetHeight();
    //   UIHelp::setScissor(renderPos.x + 5.0f, renderPos.y, visibleWidth, m_Size.y, sh);
      // glEnable(GL_SCISSOR_TEST);

       // Draw text offset by scroll
    UIHelp::DrawText(
        renderPos + glm::vec2(5.0f - m_ScrollOffset, (m_Size.y - textHeight) / 2.0f),
        displayText,
        m_TextColor
    );

    //glDisable(GL_SCISSOR_TEST);
   // UIHelp::RemoveScissor();

    // Draw cursor if focused and visible
    if (m_HasFocus && m_CursorVisible) {
        float cursorX = 5.0f;
        if (m_CursorPosition > 0) {
            cursorX += UIHelp::StrWidth(displayText.substr(0, m_CursorPosition)) - m_ScrollOffset;
        }

        // Only draw cursor if it's in the visible area
        if (cursorX >= 5.0f && cursorX <= 5.0f + visibleWidth) {
            UIHelp::DrawRect(
                renderPos + glm::vec2(cursorX, 2.0f),
                glm::vec2(1.0f, m_Size.y - 4.0f),
                m_CursorColor
            );
        }
    }

    RenderChildren();
}
void ITextEdit::OnMouseDown(int button) {
    if (button == MOUSE_BUTTON_LEFT) {
        m_HasFocus = true;

        glm::vec2 mousePos = GameInput::MousePosition - GetRenderPosition();

        // Check if we're in numeric mode and the click is in the drag area (left 5 pixels)
        if (m_IsNumeric && mousePos.x <= 5.0f) {
            m_IsDragging = true;
            m_DragStartX = GameInput::MousePosition.x;
            m_LastDragAmount = 0.0f;
            return; // Don't process this as a normal click
        }

        // Normal click handling (calculating cursor position)
        int textPos = 0;
        std::string displayText = m_IsPassword ? std::string(m_Text.length(), '*') : m_Text;

        float x = 5.0f - m_ScrollOffset;
        for (size_t i = 0; i <= displayText.length(); i++) {
            float charWidth = (i < displayText.length()) ?
                UIHelp::StrWidth(displayText.substr(i, 1)) : 0.0f;

            if (mousePos.x <= x + charWidth / 2.0f) {
                textPos = i;
                break;
            }

            x += charWidth;
            textPos = i + 1;
        }

        m_CursorPosition = std::min(textPos, (int)m_Text.length());
        m_SelectionStart = m_CursorPosition;
        m_SelectionEnd = m_CursorPosition;

        // Reset cursor blink
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
    }
}
void ITextEdit::OnMouseUp(int button) {
    if (button == MOUSE_BUTTON_LEFT && m_IsDragging) {
        m_IsDragging = false;

        // Format the numeric input when drag is complete
        if (m_IsNumeric) {
            FormatNumericInput();
        }
    }
}
void ITextEdit::OnMouseMove(glm::vec2 position, glm::vec2 delta) {
    // Handle dragging for numeric adjustment
    if (m_IsDragging && m_IsNumeric) {
        float dragDelta = GameInput::MousePosition.x - m_DragStartX;

        // Convert the current text to a float
        float currentValue = 0.0f;
        try {
            currentValue = std::stof(m_Text);
        }
        catch (...) {
            // If conversion fails, use 0.0f
            currentValue = 0.0f;
        }

        // Calculate the adjustment based on dragDelta
        float totalDragUnits = dragDelta / m_DragSensitivity;
        float adjustment = totalDragUnits * m_DragAdjustInterval - m_LastDragAmount;

        // Only apply adjustment if it's significant enough
        if (std::abs(adjustment) >= m_DragAdjustInterval) {
            // Update the value
            float newValue = currentValue + adjustment;

            // Format the new value with proper precision
            // Determine how many decimal places to show based on the interval
            int precision = 0;
            float interval = m_DragAdjustInterval;
            while (interval < 1.0f && precision < 10) {
                interval *= 10.0f;
                precision++;
            }

            // Convert to string with appropriate precision
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%.*f", precision, newValue);
            m_Text = buffer;

            // Update cursor position and selection
            m_CursorPosition = m_Text.length();
            m_SelectionStart = m_SelectionEnd = -1;

            // Update the last drag amount
            m_LastDragAmount += adjustment;

            // Call Edit callback
            if (OnEdit) {
                OnEdit(m_Text);
            }

            // Also call Enter when value changes via drag
            Enter(nullptr);
        }

        return;
    }

    // Normal mouse move handling for selection
    if (m_HasFocus && GameInput::Buttons[MOUSE_BUTTON_LEFT] && m_SelectionStart >= 0 && !m_IsDragging) {
        glm::vec2 mousePos = position;

        // Calculate cursor position based on mouse position
        int textPos = 0;
        std::string displayText = m_IsPassword ? std::string(m_Text.length(), '*') : m_Text;

        float x = 5.0f - m_ScrollOffset;
        for (size_t i = 0; i <= displayText.length(); i++) {
            float charWidth = (i < displayText.length()) ?
                UIHelp::StrWidth(displayText.substr(i, 1)) : 0.0f;

            if (mousePos.x <= x + charWidth / 2.0f) {
                textPos = i;
                break;
            }

            x += charWidth;
            textPos = i + 1;
        }

        m_CursorPosition = std::min(textPos, (int)m_Text.length());
        m_SelectionEnd = m_CursorPosition;

        // Reset cursor blink
        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;

        // Ensure cursor is visible while dragging
        EnsureCursorVisible();
    }
}


void ITextEdit::EnsureCursorVisible() {
    if (m_CursorPosition <= 0) {
        m_ScrollOffset = 0.0f;
        return;
    }

    std::string displayText = m_IsPassword ? std::string(m_Text.length(), '*') : m_Text;
    float textWidth = UIHelp::StrWidth(displayText);
    float visibleWidth = m_Size.x - 10.0f; // Padding

    float cursorX = UIHelp::StrWidth(displayText.substr(0, m_CursorPosition));

    if (cursorX - m_ScrollOffset < 0) {
        m_ScrollOffset = cursorX - 5.0f;
    }
    else if (cursorX - m_ScrollOffset > visibleWidth - 5.0f) {
        m_ScrollOffset = cursorX - visibleWidth + 5.0f;
    }

    m_ScrollOffset = std::max(0.0f, std::min(m_ScrollOffset, textWidth - visibleWidth));
    if (textWidth < visibleWidth) {
        m_ScrollOffset = 0.0f;
    }
}

// Implement the HandleKeyWithRepeat function
bool ITextEdit::HandleKeyWithRepeat(int key) {
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

// Update the Update method to track key timings
void ITextEdit::Update(float delta) {
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

    UpdateChildren(delta);
}

// Modify HandleKeyInput method to support numeric mode
void ITextEdit::HandleKeyInput() {
    if (m_IsReadOnly) return;

    bool ctrl = GameInput::Keys[GLFW_KEY_LEFT_CONTROL] || GameInput::Keys[GLFW_KEY_RIGHT_CONTROL];
    bool shift = GameInput::Keys[GLFW_KEY_LEFT_SHIFT] || GameInput::Keys[GLFW_KEY_RIGHT_SHIFT];

    // Handle navigation keys with repeat
    if (HandleKeyWithRepeat(GLFW_KEY_LEFT)) {
        if (ctrl) {
            // Move to previous word
            int pos = m_CursorPosition - 1;
            while (pos > 0 && !isalnum(m_Text[pos - 1])) pos--;
            while (pos > 0 && isalnum(m_Text[pos - 1])) pos--;
            m_CursorPosition = pos;
        }
        else {
            // Move left one character
            m_CursorPosition = std::max(0, m_CursorPosition - 1);
        }

        // Update selection if shift is pressed
        if (shift) {
            if (m_SelectionStart == -1) m_SelectionStart = m_CursorPosition + 1;
            m_SelectionEnd = m_CursorPosition;
        }
        else {
            m_SelectionStart = m_SelectionEnd = -1;
        }

        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
        EnsureCursorVisible();
    }

    if (HandleKeyWithRepeat(GLFW_KEY_RIGHT)) {
        if (ctrl) {
            // Move to next word
            int pos = m_CursorPosition;
            while (pos < m_Text.length() && !isalnum(m_Text[pos])) pos++;
            while (pos < m_Text.length() && isalnum(m_Text[pos])) pos++;
            m_CursorPosition = pos;
        }
        else {
            // Move right one character
            m_CursorPosition = std::min((int)m_Text.length(), m_CursorPosition + 1);
        }

        // Update selection if shift is pressed
        if (shift) {
            if (m_SelectionStart == -1) m_SelectionStart = m_CursorPosition - 1;
            m_SelectionEnd = m_CursorPosition;
        }
        else {
            m_SelectionStart = m_SelectionEnd = -1;
        }

        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
        EnsureCursorVisible();
    }

    if (HandleKeyWithRepeat(GLFW_KEY_HOME)) {
        m_CursorPosition = 0;

        // Update selection if shift is pressed
        if (shift) {
            if (m_SelectionStart == -1) m_SelectionStart = m_CursorPosition;
            m_SelectionEnd = 0;
        }
        else {
            m_SelectionStart = m_SelectionEnd = -1;
        }

        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
        EnsureCursorVisible();
    }

    if (HandleKeyWithRepeat(GLFW_KEY_END)) {
        m_CursorPosition = m_Text.length();

        // Update selection if shift is pressed
        if (shift) {
            if (m_SelectionStart == -1) m_SelectionStart = m_CursorPosition;
            m_SelectionEnd = m_Text.length();
        }
        else {
            m_SelectionStart = m_SelectionEnd = -1;
        }

        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
        EnsureCursorVisible();
    }

    // Handle text editing keys with repeat
    if (HandleKeyWithRepeat(GLFW_KEY_DELETE)) {
        bool textChanged = false;

        if (m_SelectionStart != m_SelectionEnd && m_SelectionStart != -1) {
            // Delete selection
            int start = std::min(m_SelectionStart, m_SelectionEnd);
            int end = std::max(m_SelectionStart, m_SelectionEnd);
            m_Text.erase(start, end - start);
            m_CursorPosition = start;
            m_SelectionStart = m_SelectionEnd = -1;
            textChanged = true;
        }
        else if (m_CursorPosition < m_Text.length()) {
            // Delete character after cursor
            m_Text.erase(m_CursorPosition, 1);
            textChanged = true;
        }

        if (textChanged) {
            Enter(nullptr);
        }

        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
        EnsureCursorVisible();
    }

    if (HandleKeyWithRepeat(GLFW_KEY_BACKSPACE)) {
        bool textChanged = false;

        if (m_SelectionStart != m_SelectionEnd && m_SelectionStart != -1) {
            // Delete selection
            int start = std::min(m_SelectionStart, m_SelectionEnd);
            int end = std::max(m_SelectionStart, m_SelectionEnd);
            m_Text.erase(start, end - start);
            m_CursorPosition = start;
            m_SelectionStart = m_SelectionEnd = -1;
            textChanged = true;
        }
        else if (m_CursorPosition > 0) {
            // Delete character before cursor
            m_Text.erase(m_CursorPosition - 1, 1);
            m_CursorPosition--;
            textChanged = true;
        }

        if (textChanged) {
            Enter(nullptr);
        }

        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
        EnsureCursorVisible();
    }

    // Handle Ctrl+A (select all) - This doesn't typically repeat
    if (ctrl && GameInput::Keys[GLFW_KEY_A]) {
        GameInput::Keys[GLFW_KEY_A] = false; // Consume the key press

        m_SelectionStart = 0;
        m_SelectionEnd = m_Text.length();
        m_CursorPosition = m_Text.length();

        m_CursorBlinkTime = 0.0f;
        m_CursorVisible = true;
        EnsureCursorVisible();
        return; // Don't insert 'A'
    }

    if (GameInput::Keys[GLFW_KEY_ENTER])
    {
        // In numeric mode, format the number
        if (m_IsNumeric) {
            FormatNumericInput();
        }

        Enter(m_Data);
        GameInput::Keys[GLFW_KEY_ENTER] = false; // Consume the key press
    }

    // Handle regular character input with repeat
    for (int key = 32; key <= 126; key++) { // ASCII printable characters
        if (HandleKeyWithRepeat(key)) {
            if (ctrl) continue; // Skip if ctrl is held (control characters)

            if (m_MaxLength < 0 || (int)m_Text.length() < m_MaxLength) {
                if (m_SelectionStart != m_SelectionEnd && m_SelectionStart != -1) {
                    // Replace selection with character
                    int start = std::min(m_SelectionStart, m_SelectionEnd);
                    int end = std::max(m_SelectionStart, m_SelectionEnd);
                    m_Text.erase(start, end - start);
                    m_CursorPosition = start;
                    m_SelectionStart = m_SelectionEnd = -1;
                }

                // Get the character
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

                // For numeric mode, check if character is valid
                if (m_IsNumeric) {
                    bool decimalExists = false;
                    if (!IsValidNumericInput(c, decimalExists)) {
                        continue; // Skip invalid characters in numeric mode
                    }
                }

                m_Text.insert(m_CursorPosition, 1, c);
                m_CursorPosition++;

                // Call Enter when text changes
                Enter(nullptr);

                m_CursorBlinkTime = 0.0f;
                m_CursorVisible = true;
                EnsureCursorVisible();
            }
        }
    }
}

void ITextEdit::OnActivate() {
    this->SetFocus(true);
}

void ITextEdit::OnDeactivate() {
    // In numeric mode, format the number
    if (m_IsNumeric) {
        FormatNumericInput();
    }

    this->SetFocus(false);
}

bool ITextEdit::IsValidNumericInput(char c, bool& decimalExists) {
    // If it's a digit, it's always valid
    if (c >= '0' && c <= '9') {
        return true;
    }

    // If it's a decimal point, check if one already exists
    if (c == '.') {
        decimalExists = false;

        // Check if the text already contains a decimal point
        for (size_t i = 0; i < m_Text.length(); i++) {
            if (m_Text[i] == '.') {
                decimalExists = true;
                break;
            }
        }

        // Allow only if no decimal point exists yet
        return !decimalExists;
    }

    // Allow minus sign only at the beginning
    if (c == '-') {
        return m_CursorPosition == 0 && m_Text.find('-') == std::string::npos;
    }

    // Any other character is invalid in numeric mode
    return false;
}

// Add this helper method to format numeric input
void ITextEdit::FormatNumericInput() {
    bool changed = false;

    // If field is empty or just a minus sign, set it to "0"
    if (m_Text.empty() || m_Text == "-") {
        m_Text = "0";
        m_CursorPosition = 1;
        changed = true;
    }
    // If field starts with a decimal point, add "0" in front
    else if (m_Text.length() > 0 && m_Text[0] == '.') {
        m_Text = "0" + m_Text;
        m_CursorPosition++;
        changed = true;
    }
    // If field starts with minus and then decimal, add "0" between them
    else if (m_Text.length() > 1 && m_Text[0] == '-' && m_Text[1] == '.') {
        m_Text.insert(1, "0");
        m_CursorPosition++;
        changed = true;
    }

    // Handle multiple leading zeros
    if (m_Text.length() > 1) {
        if (m_Text[0] == '0') {
            // Check if we have multiple leading zeros
            size_t pos = 0;
            while (pos < m_Text.length() - 1 && m_Text[pos] == '0' && m_Text[pos + 1] == '0') {
                m_Text.erase(pos, 1);
                if (m_CursorPosition > pos) {
                    m_CursorPosition--;
                }
                changed = true;
            }

            // If we have a non-decimal digit after leading zeros, keep only one zero
            if (pos < m_Text.length() - 1 && m_Text[pos] == '0' && m_Text[pos + 1] != '.' && isdigit(m_Text[pos + 1])) {
                m_Text.erase(pos, 1);
                if (m_CursorPosition > pos) {
                    m_CursorPosition--;
                }
                changed = true;
            }
        }
        else if (m_Text[0] == '-' && m_Text.length() > 2) {
            // Handle negative numbers with multiple leading zeros
            size_t pos = 1;
            while (pos < m_Text.length() - 1 && m_Text[pos] == '0' && m_Text[pos + 1] == '0') {
                m_Text.erase(pos, 1);
                if (m_CursorPosition > pos) {
                    m_CursorPosition--;
                }
                changed = true;
            }

            // If we have a non-decimal digit after leading zeros, keep only one zero
            if (pos < m_Text.length() - 1 && m_Text[pos] == '0' && m_Text[pos + 1] != '.' && isdigit(m_Text[pos + 1])) {
                m_Text.erase(pos, 1);
                if (m_CursorPosition > pos) {
                    m_CursorPosition--;
                }
                changed = true;
            }
        }
    }

    // Ensure cursor position is valid
    m_CursorPosition = std::max(0, std::min(m_CursorPosition, (int)m_Text.length()));

    // Trigger edit callback if changes were made
    if (changed && OnEdit) {
        OnEdit(m_Text);
    }
}

void ITextEdit::SetNumber(float number) {
    // Only proceed if in numeric mode
    if (!m_IsNumeric) {
        return;
    }

    // Convert the number to a string with appropriate precision
    // Determine how many decimal places to show based on the drag adjust interval
    int precision = 0;
    float interval = m_DragAdjustInterval;
    while (interval < 1.0f && precision < 10) {
        interval *= 10.0f;
        precision++;
    }

    // Convert to string with appropriate precision
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.*f", precision, number);
    m_Text = buffer;

    // Apply standard numeric formatting rules
    FormatNumericInput();

    // Set cursor at the end of the text
    m_CursorPosition = m_Text.length();
    m_SelectionStart = m_SelectionEnd = -1;

    // Reset cursor blink
    m_CursorBlinkTime = 0.0f;
    m_CursorVisible = true;

    // Call the edit callback
    if (OnEdit) {
        OnEdit(m_Text);
    }
}

float ITextEdit::GetNumber() const {
    // Only proceed if in numeric mode
    if (!m_IsNumeric) {
        return 0.0f;
    }

    // Try to convert the current text to a float
    try {
        return std::stof(m_Text);
    }
    catch (const std::exception& e) {
        // If conversion fails (invalid format), return 0.0f
        return 0.0f;
    }
}