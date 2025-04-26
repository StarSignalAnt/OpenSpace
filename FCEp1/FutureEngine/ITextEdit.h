#pragma once
#include "IControl.h"
#include <string>
#include <unordered_map>

class ITextEdit : public IControl
{
public:
    ITextEdit();
    ITextEdit(glm::vec2 position, glm::vec2 size);

    virtual void Update(float delta) override;
    virtual void Render() override;

    virtual void OnMouseDown(int button) override;
    virtual void OnMouseUp(int button) override;
    virtual void OnMouseMove(glm::vec2 position, glm::vec2 delta) override;
    void OnActivate() override;
    void OnDeactivate() override;

    // Set if this control has keyboard focus
    void SetFocus(bool focus) { m_HasFocus = focus; }
    bool HasFocus() const { return m_HasFocus; }

    // Text properties
    void SetIsPassword(bool isPassword) { m_IsPassword = isPassword; }
    bool GetIsPassword() const { return m_IsPassword; }

    // Numeric mode property
    void SetIsNumeric(bool isNumeric) {
        m_IsNumeric = isNumeric;
        if (m_IsNumeric && (m_Text.empty() || m_Text == "-")) {
            m_Text = "0";
            m_CursorPosition = 1;
        }
    }
    bool GetIsNumeric() const { return m_IsNumeric; }

    void SetMaxLength(int maxLength) { m_MaxLength = maxLength; }
    int GetMaxLength() const { return m_MaxLength; }

    void SetIsReadOnly(bool isReadOnly) { m_IsReadOnly = isReadOnly; }
    bool GetIsReadOnly() const { return m_IsReadOnly; }

    // Customization properties
    void SetTextColor(glm::vec4 color) { m_TextColor = color; }
    void SetBackgroundColor(glm::vec4 color) { m_BackgroundColor = color; }
    void SetBorderColor(glm::vec4 color) { m_BorderColor = color; }
    void SetCursorColor(glm::vec4 color) { m_CursorColor = color; }

    // Key repeat settings
    void SetKeyRepeatDelay(float delay) { m_KeyRepeatDelay = delay; }
    void SetKeyRepeatInterval(float interval) { m_KeyRepeatInterval = interval; }

    void SetOnEdit(std::function<void(std::string text)> edit) {
        OnEdit = edit;
    }

    void Edited() {
        if (OnEdit) {
            OnEdit(m_Text);
        }
    }

    // Drag adjustment for numeric mode
    void SetDragAdjustInterval(float interval) { m_DragAdjustInterval = interval; }
    float GetDragAdjustInterval() const { return m_DragAdjustInterval; }

    void SetDragSensitivity(float sensitivity) { m_DragSensitivity = sensitivity; }
    float GetDragSensitivity() const { return m_DragSensitivity; }

    void Reset() {
        if (m_IsNumeric) {
            m_Text = "0";
            m_CursorPosition = 1;
        }
        else {
            m_Text = "";
            m_CursorPosition = 0;
        }
    }

    // Set a numeric value (only works when IsNumeric is true)
    void SetNumber(float number);

    // Get the current numeric value (only works when IsNumeric is true)
    float GetNumber() const;

private:
    void HandleKeyInput();
    void EnsureCursorVisible();
    bool HandleKeyWithRepeat(int key);
    bool IsValidNumericInput(char c, bool& decimalExists);
    void FormatNumericInput();  // New helper method

    int m_CursorPosition = 0;
    int m_SelectionStart = -1;
    int m_SelectionEnd = -1;
    float m_ScrollOffset = 0.0f;

    float m_CursorBlinkTime = 0.0f;
    bool m_CursorVisible = true;
    bool m_HasFocus = false;

    bool m_IsPassword = false;
    bool m_IsNumeric = false;   // Numeric mode flag
    bool m_IsReadOnly = false;
    int m_MaxLength = -1;

    // Drag adjustment variables
    bool m_IsDragging = false;
    float m_DragStartX = 0.0f;
    float m_DragAdjustInterval = 0.1f;  // Default adjustment amount per drag unit
    float m_DragSensitivity = 5.0f;     // Pixels of movement needed for one adjustment
    float m_LastDragAmount = 0.0f;      // Track accumulated drag amount

    // Key repeat settings
    float m_KeyRepeatDelay = 0.5f;      // Initial delay in seconds before repeating
    float m_KeyRepeatInterval = 0.03f;   // Interval between repeats in seconds

    struct KeyRepeatInfo {
        bool isPressed = false;
        float timePressed = 0.0f;
        float timeSinceRepeat = 0.0f;
        bool hasRepeated = false;
    };

    std::unordered_map<int, KeyRepeatInfo> m_KeyStates;

    glm::vec4 m_TextColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 m_BackgroundColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    glm::vec4 m_BorderColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    glm::vec4 m_SelectionColor = glm::vec4(0.3f, 0.5f, 0.7f, 0.5f);
    glm::vec4 m_CursorColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    std::function<void(std::string)> OnEdit = nullptr;
};