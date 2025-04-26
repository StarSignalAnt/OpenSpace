#pragma once
#include "IControl.h"
#include "GameUI.h"
#include <string>
#include <vector>
#include "UIHelp.h"
#include "UITheme.h"
#include "IControlGroup.h"
#include "IImage.h"
#include "ICheckBox.h"
#include "ITextEdit.h"
#include "IButton.h"

enum PropertyType {
    PT_Image,PT_Vec2,PT_Vec3,PT_Vec4,PT_Check,PT_Text,PT_Float
};

class PropertyItem : public IControl {
public:

    PropertyItem(std::string name,PropertyType type) {
        m_Text = name;
        m_Editor = new IControlGroup();

        switch (type) {
        case PT_Float:
            m_Float = new ITextEdit(glm::vec2(0, 1), glm::vec2(60, 24));
            m_Editor->AddChild(m_Float);
            m_Float->SetIsNumeric(true);
            break;
        case PT_Vec3:

            m_VX = new ITextEdit(glm::vec2(0, 1), glm::vec2(60, 24));
            m_Editor->AddChild(m_VX);
            m_VY = new ITextEdit(glm::vec2(65, 1), glm::vec2(60, 24));
            m_VZ = new ITextEdit(glm::vec2(130, 1), glm::vec2(60, 24));
            m_Editor->AddChild(m_VY);
            m_Editor->AddChild(m_VZ);
            m_VX->SetIsNumeric(true);
            m_VY->SetIsNumeric(true);
            m_VZ->SetIsNumeric(true);


            break;
        case PT_Text:
            m_PropText = new ITextEdit(glm::vec2(0, 1),glm::vec2(220,27));
            m_Editor->AddChild(m_PropText);
            break;
        case PT_Check:
                m_PropCheck = new ICheckBox(glm::vec2(0, 3));
                m_Editor->AddChild(m_PropCheck);
                //m_PropImage->SetImage()
             //   m_PropImage->SetIgnoreScissor(true);
                break;
        case PT_Image:
        {
            m_PropImage = new IImage(glm::vec2(0, 0), glm::vec2(28, 28));
            m_ImgDot = new IButton("...", glm::vec2(37, 15), glm::vec2(25, 15));
            m_Editor->AddChild(m_PropImage);
            m_Editor->AddChild(m_ImgDot);
                //m_PropImage->SetImage()
            m_PropImage->SetIgnoreScissor(true);
            
    
        }

            break;
        }
        AddChild(m_Editor);
      //  m_Editor->SetIgnoreScissor(true);

        
    }

    void Update(float delta) override {
        m_Size.x = GetRoot()->GetSize().x;
        UpdateChildren(delta);
    }

    void SetEditor(IControl* control)
    {

    }

    void SetImage(Texture2D* image) {

        m_PropImage->SetImage(image);

    }

    void Render() override {
        m_Size.x = GetRoot()->GetSize().x;
        auto theme = GameUI::GetTheme();
        UIHelp::DrawRect(GetRenderPosition() + glm::vec2(150, -2), glm::vec2(2, 32),theme->GetWindowDark());
        UIHelp::DrawText(GetRenderPosition() + glm::vec2(5, 5), m_Text, glm::vec4(1, 1, 1, 1));
        UIHelp::DrawRect(GetRenderPosition() + glm::vec2(0, 30), glm::vec2(m_Size.x, 2), theme->GetWindowDark());
        m_Editor->Set(glm::vec2(160, 0), glm::vec2(m_Size.x-130, 28));
        RenderChildren();
    }
    IImage* m_PropImage;
    ICheckBox* m_PropCheck;
    ITextEdit* m_PropText;
    IControlGroup* m_Editor;
    ITextEdit* m_VX,* m_VY,* m_VZ;
    IButton* m_ImgDot;
    ITextEdit* m_Float;
     

};

class PropertyGroup : public IControl {
public:
    PropertyGroup(std::string name) {
        m_Name = name;
        SetDockType(m_Centre);
    }
    std::string m_Name;
    bool m_Open = true;
    void Update(float delta) override {
        UpdateChildren(delta);
    }
    void Render() override {

        auto theme = GameUI::GetTheme();

        UIHelp::DrawRect(GetRenderPosition(), glm::vec2(m_Size.x,30), theme->GetWindowLight()*2.0f);
        UIHelp::DrawText(GetRenderPosition() + glm::vec2(5, 8), m_Name, glm::vec4(1, 1, 1, 1),1.1);
        RenderChildren();



    }
    PropertyItem* AddItem(PropertyItem* item) {
        m_Items.push_back(item);
        AddChild(item);

        int sy = m_Items.size() * 34;

        item->Set(glm::vec2(0, sy+5), glm::vec2(m_Size.x, 30));
        m_Size = glm::vec2(m_Size.x, 30 + sy);
        return item;
    }

    std::vector<PropertyItem*> m_Items;

};

class IPropertyEditor :
    public IControl
{
public:

    IPropertyEditor() : IControl(glm::vec2(0, 0), glm::vec2(0, 0))
    {
        SetDockType(DockType::m_Fill);
    }
    void Update(float delta) override;
    void Render() override;
    void AddGroup(PropertyGroup* group);

private:

    std::vector<PropertyGroup*> m_Groups;

};

