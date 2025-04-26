#pragma once
#include "IWindow.h"
#include "ILabel.h"
#include "ITextEdit.h"
#include "IButton.h"
#include "GameUI.h"
#include <sstream>
#include <functional>
#include "UIHelp.h"
#include "FutureApp.h"

class IGetName : public IWindow {
public:
    IGetName(std::string prompt, std::string title, std::function<void(std::string)> onConfirm = nullptr);
    ~IGetName() = default;

private:
    void CenterOnScreen();

    std::function<void(std::string)> m_OnConfirm;
    ILabel* m_PromptLabel;
    ITextEdit* m_TextInput;
    IButton* m_OkButton;
    IButton* m_CancelButton;
};