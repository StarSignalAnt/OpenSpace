#pragma once
#include "IControl.h"
class ICheckBox :
    public IControl
{
public:

    ICheckBox(glm::vec2 position) : IControl(position, glm::vec2(128, 32))
    {

    }

    void Update(float delta) override;
    void Render() override;

    bool GetChecked() {
        return m_Checked;
    }
    void SetChecked(bool checked) {
        m_Checked = checked;
    }
    void OnMouseDown(int button);
    
    void Checked() {

        if (OnCheck) {
            OnCheck(m_Checked);
        }

    }
    
    void SetOnCheck(std::function<void(bool)> check)
    {
        OnCheck = check;
    }


private:

    std::function<void(bool check)> OnCheck = nullptr;
    bool m_Checked = false;

};

