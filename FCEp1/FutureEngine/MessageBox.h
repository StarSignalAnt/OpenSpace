#pragma once
#include "IWindow.h"
#include "ILabel.h"
#include "IButton.h"
#include "GameUI.h"
#include <sstream>
#include <functional>
#include "UIHelp.h"
#include "FutureApp.h"

class MessageBox : public IWindow {
public:
    MessageBox(std::string message,std::string title, std::function<void()> onClose = nullptr);
    ~MessageBox() = default;

private:
    void CenterOnScreen();
    std::vector<std::string> SplitText(const std::string& text, float maxWidth);

    std::function<void()> m_OnClose;
    std::vector<ILabel*> m_TextLabels;
    IButton* m_OkButton;
};

// Implementation
