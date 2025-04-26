#pragma once
#include "IControl.h"
#include "IVerticalScroller.h"
#include "IHorizontalScroller.h"
#include <string>
#include <vector>
#include <unordered_map>

class ITextArea : public IControl
{
public:
    ITextArea();
    ITextArea(glm::vec2 position, glm::vec2 size);

    virtual void Update(float delta) override;
    virtual void Render() override;
    virtual void PreRender() override;

    virtual void OnMouseDown(int button) override;
    virtual void OnMouseUp(int button) override;
    virtual void OnMouseMove(glm::vec2 position, glm::vec2 delta) override;
    virtual void OnActivate() override;
    virtual void OnDeactivate() override;

    // Set if this control has keyboard focus
    void SetFocus(bool focus);
    bool HasFocus() const { return m_HasFocus; }

    // Text properties
    void SetText(const std::string& text);
    std::string GetText() const;
    void SetIsReadOnly(bool isReadOnly) { m_IsReadOnly = isReadOnly; }
    bool GetIsReadOnly() const { return m_IsReadOnly; }

    // Cursor and selection
    void SetCursorPosition(int line, int column);
    std::pair<int, int> GetCursorPosition() const; // Returns {line, column}
    void SelectText(int startLine, int startCol, int endLine, int endCol);
    void ClearSelection();
    std::string GetSelectedText() const;

    // Customization properties
    void SetTextColor(glm::vec4 color) { m_TextColor = color; }
    void SetBackgroundColor(glm::vec4 color) { m_BackgroundColor = color; }
    void SetBorderColor(glm::vec4 color) { m_BorderColor = color; }
    void SetCursorColor(glm::vec4 color) { m_CursorColor = color; }
    void SetSelectionColor(glm::vec4 color) { m_SelectionColor = color; }
    void SetLineHeight(float height) { m_LineHeight = height; }
    void SetPadding(float padding) { m_Padding = padding; }

    // Key repeat settings
    void SetKeyRepeatDelay(float delay) { m_KeyRepeatDelay = delay; }
    void SetKeyRepeatInterval(float interval) { m_KeyRepeatInterval = interval; }

    // Scrolling
    void ScrollToPosition(float verticalPos, float horizontalPos);
    void ScrollToCursor();
    void SetAllowHorizontalScroll(bool allow) { m_AllowHorizontalScroll = allow; }

    // Events
    void SetOnTextChanged(std::function<void(const std::string&)> callback) { OnTextChanged = callback; }
    // Text properties

    void AddText(const std::string& text);


private:
    // Text handling
    void HandleKeyInput();
    bool HandleKeyWithRepeat(int key);
    void ProcessTextInput(char c);
    void SplitTextIntoLines();
    void UpdateContentDimensions();
    float GetLineWidth(const std::string& line) const;
    int GetCharacterIndexAtPosition(int lineIndex, glm::vec2 position) const;
    void EnsureCursorVisible();
    void DeleteSelectedText();
    void RebuildText();


    // Rendering
    void DrawContent();
    void DrawSelectionHighlights();
    void DrawCursor();

    // Key repeat handling
    struct KeyRepeatInfo {
        bool isPressed = false;
        float timePressed = 0.0f;
        float timeSinceRepeat = 0.0f;
        bool hasRepeated = false;
    };
    std::unordered_map<int, KeyRepeatInfo> m_KeyStates;

    // Text storage and state
    std::string m_Text;
    std::vector<std::string> m_Lines;
    int m_CursorLine = 0;
    int m_CursorColumn = 0;
    int m_SelectionStartLine = -1;
    int m_SelectionStartColumn = -1;
    int m_SelectionEndLine = -1;
    int m_SelectionEndColumn = -1;

    // UI state
    bool m_HasFocus = false;
    bool m_IsReadOnly = false;
    float m_CursorBlinkTime = 0.0f;
    bool m_CursorVisible = true;
    bool m_IsDragging = false;
    bool m_AllowHorizontalScroll = true;

    // Scrolling
    IVerticalScroller* m_VerticalScroller = nullptr;
    IHorizontalScroller* m_HorizontalScroller = nullptr;
    float m_VerticalScrollOffset = 0.0f;
    float m_HorizontalScrollOffset = 0.0f;
    float m_MaxContentWidth = 0.0f;
    float m_MaxContentHeight = 0.0f;

    // Styling
    float m_LineHeight = 18.0f;
    float m_Padding = 5.0f;
    glm::vec4 m_TextColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 m_BackgroundColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    glm::vec4 m_BorderColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    glm::vec4 m_SelectionColor = glm::vec4(0.3f, 0.5f, 0.7f, 0.5f);
    glm::vec4 m_CursorColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    // Key repeat settings
    float m_KeyRepeatDelay = 0.5f;      // Initial delay in seconds before repeating
    float m_KeyRepeatInterval = 0.03f;   // Interval between repeats in seconds

    // Event callbacks
    std::function<void(const std::string&)> OnTextChanged = nullptr;
};