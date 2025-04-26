#pragma once
#include "IControl.h"

class IButton;
class Texture2D;

class IToolBar :
    public IControl
{
public:

    IToolBar() {

    };
    void Update(float delta) override;
    void Render() override;
    IButton* AddButton(std::string text);
    IButton* AddButton(Texture2D* icon);
    void Seperator(int width);
private:

    int m_NextX = 25;

};

