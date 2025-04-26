#pragma once
#include "UITheme.h"
class ThemeDark :
    public UITheme
{
public:

    ThemeDark() {
        SetupTheme();
    }
    void SetupTheme() override;

};

