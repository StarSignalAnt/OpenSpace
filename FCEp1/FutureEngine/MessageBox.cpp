#include "MessageBox.h"

MessageBox::MessageBox(std::string message,std::string title, std::function<void()> onClose)
    : IWindow(title, glm::vec2(0, 0), glm::vec2(400, 200)), // Initial size, will be adjusted
    m_OnClose(onClose)
{
    // Calculate appropriate width based on text
    float textWidth = UIHelp::StrWidth(message);
    float maxWidth = FutureApp::m_Inst->GetWidth() * 0.7f; // Max 70% of screen width
    maxWidth = std::min(maxWidth, 600.0f); // Cap at 600 pixels

    // Calculate appropriate height (at least enough for 3-4 lines)
    float lineHeight = UIHelp::StrHeight("Test") * 1.5f; // 1.5x for spacing

    // Split text into lines if needed
    std::vector<std::string> lines = SplitText(message, maxWidth - 120); // Account for padding

    // Calculate final window dimensions
    int finalWidth = std::max(int(maxWidth), 300); // Minimum width is 300
    int finalHeight = std::max(int(lines.size() * lineHeight + 80), 150); // Extra space for button and padding

    // Resize window
    Set(glm::vec2(0, 0), glm::vec2(finalWidth, finalHeight));

    // Add text labels to window
    float yPos = 30;
    for (const auto& line : lines) {
        ILabel* label = new ILabel(line, glm::vec2(20, yPos));
        AddClientControl(label);
        m_TextLabels.push_back(label);
        yPos += lineHeight;
    }

    // Add OK button
    m_OkButton = new IButton("OK", glm::vec2((finalWidth - 80) / 2, finalHeight - 60), glm::vec2(80, 30));
    m_OkButton->SetOnClick([this](void* data) {
        // Remove from parent and call callback if provided
        if (GetRoot()) {
            GetRoot()->RemoveChild(this);
        }
        if (m_OnClose) {
            m_OnClose();
        }
        });
    AddClientControl(m_OkButton);

    // Center on screen
    CenterOnScreen();

    // Add to GameUI
    GameUI::m_Inst->GetRoot()->AddChild(this);
}

void MessageBox::CenterOnScreen() {
    int screenWidth = FutureApp::m_Inst->GetWidth();
    int screenHeight = FutureApp::m_Inst->GetHeight();

    // Center position
    Set(glm::vec2((screenWidth - m_Size.x) / 2, (screenHeight - m_Size.y) / 2), m_Size);
}

std::vector<std::string> MessageBox::SplitText(const std::string& text, float maxWidth) {
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string line;
    std::string currentLine;

    // Split by newlines first
    while (std::getline(stream, line)) {
        std::istringstream lineStream(line);
        std::string word;

        // Process each word in the line
        while (lineStream >> word) {
            // Test if word fits on current line
            std::string testLine = currentLine;
            if (!currentLine.empty()) {
                testLine += " ";
            }
            testLine += word;

            if (UIHelp::StrWidth(testLine) <= maxWidth) {
                // Word fits on current line
                currentLine = testLine;
            }
            else {
                // Word doesn't fit - create new line
                if (!currentLine.empty()) {
                    lines.push_back(currentLine);
                    currentLine = word;
                }
                else {
                    // Current line is empty, word is too long
                    lines.push_back(word);
                }
            }
        }

        // End of line - add current line and start new one
        if (!currentLine.empty()) {
            lines.push_back(currentLine);
            currentLine.clear();
        }
        else if (lines.empty()) {
            // Empty line
            lines.push_back("");
        }
    }

    // If no lines were added, add the original text as a single line
    if (lines.empty() && !text.empty()) {
        lines.push_back(text);
    }

    return lines;
}