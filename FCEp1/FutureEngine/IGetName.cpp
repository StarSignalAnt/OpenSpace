#include "IGetName.h"

IGetName::IGetName(std::string prompt, std::string title, std::function<void(std::string)> onConfirm)
    : IWindow(title, glm::vec2(0, 0), glm::vec2(400, 150)), // Initial size
    m_OnConfirm(onConfirm)
{
    // Calculate appropriate width based on prompt text
    float textWidth = UIHelp::StrWidth(prompt);
    float maxWidth = FutureApp::m_Inst->GetWidth() * 0.7f; // Max 70% of screen width
    maxWidth = std::min(maxWidth, 600.0f); // Cap at 600 pixels

    // Calculate final window dimensions
    int finalWidth = std::max(int(maxWidth) + 40, 300); // Minimum width is 300
    int finalHeight = 180; // Fixed height for input dialog

    // Resize window
    Set(glm::vec2(0, 0), glm::vec2(finalWidth, finalHeight));

    // Add prompt label
    m_PromptLabel = new ILabel(prompt, glm::vec2(20, 30));
    AddClientControl(m_PromptLabel);

    // Add text input field
    m_TextInput = new ITextEdit(glm::vec2(20, 60), glm::vec2(finalWidth - 50, 30));
    AddClientControl(m_TextInput);

    // Set initial focus to the text input
    m_TextInput->SetFocus(true);

    // Setup text input to respond to Enter key
    m_TextInput->SetOnEnter([this](void* data) {
        if (m_OnConfirm) {
            m_OnConfirm(m_TextInput->GetText());
        }
        // Close the dialog
        if (GetRoot()) {
            GetRoot()->RemoveChild(this);
        }
        });

    // Add OK button
    int buttonWidth = 80;
    int buttonHeight = 30;
    int buttonSpacing = 20;

    m_OkButton = new IButton("OK",
        glm::vec2(finalWidth - buttonWidth * 2 - buttonSpacing - 20, finalHeight - 60),
        glm::vec2(buttonWidth, buttonHeight));

    m_OkButton->SetOnClick([this](void* data) {
        // Call the callback with the entered text
        if (m_OnConfirm) {
            m_OnConfirm(m_TextInput->GetText());
        }
        // Close the dialog
        if (GetRoot()) {
            GetRoot()->RemoveChild(this);
        }
        });
    AddClientControl(m_OkButton);

    // Add Cancel button
    m_CancelButton = new IButton("Cancel",
        glm::vec2(finalWidth - buttonWidth - 20, finalHeight - 60),
        glm::vec2(buttonWidth, buttonHeight));

    m_CancelButton->SetOnClick([this](void* data) {
        // Just close the dialog without calling the callback
        if (GetRoot()) {
            GetRoot()->RemoveChild(this);
        }
        });
    AddClientControl(m_CancelButton);

    // Center on screen
    CenterOnScreen();

    // Add to GameUI
    GameUI::m_Inst->GetWindowSurface()->AddChild(this);
}

void IGetName::CenterOnScreen() {
    int screenWidth = FutureApp::m_Inst->GetWidth();
    int screenHeight = FutureApp::m_Inst->GetHeight();

    // Center position
    Set(glm::vec2((screenWidth - m_Size.x) / 2, (screenHeight - m_Size.y) / 2), m_Size);
}