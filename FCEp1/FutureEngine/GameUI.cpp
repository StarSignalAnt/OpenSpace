#include "GameUI.h"
#include "IControl.h"
#include "IControlGroup.h"
#include "GameInput.h"
#include "FutureApp.h"
#include "IDocker.h"
#include "IWindow.h"
#include "IMainMenu.h"
#include "ThemeDark.h"
#include <iostream>
#include "UIHelp.h"

GameUI* GameUI::m_Inst = nullptr;



std::vector<IControl*> AddControls(std::vector<IControl*> list, IControl* control) {

	list.push_back(control);
	for (auto sub_control : control->GetChildren())
	{
		list = AddControls(list, sub_control);
	}

	return list;
}

GameUI::GameUI()
{
    m_Inst = this;
    m_RootControl = new IControlGroup;
    m_RootControl->Set(glm::vec2(0,0), glm::vec2(FutureApp::m_Inst->GetWidth(), FutureApp::m_Inst->GetHeight()));
   // m_ActiveMenu = new IMainMenu;
    m_Theme = new ThemeDark;


}
    
bool MouseB[64] = { 0 };

void GameUI::UpdateUI(float delta)
{
    // Process any pending undock operations first


    std::vector<IControl*> list;

    list = AddControls(list, m_RootControl);

    if (m_ActiveMenu) {
        list = AddControls(list, m_ActiveMenu);
    }

    std::reverse(list.begin(), list.end()); // Reverse the vector in-place

    auto mouse_pos = GameInput::MousePosition;

 //   std::cout << " UIDelta:" << delta << std::endl;



    for (auto control : list) {

        continue;
        if (control->GetTransist())
        {
            
            auto position = control->GetPosition();
            auto size = control->GetSize();
            if (delta > 1) {
                int aa = 5;
            }
            position += (control->GetTargetPosition() - control->GetPosition()) *0.8f * (delta*22);
            size += (control->GetTargetSize() - control->GetSize()) * 0.8f * (delta*22);

            control->Set(position, size);

            glm::vec2 dif = control->GetPosition() - control->GetTargetPosition();
            
            
            float xd = control->GetTargetPosition().x - control->GetPosition().x;
            float yd = control->GetTargetPosition().y - control->GetPosition().y;

            float dis = sqrt(xd * xd + yd * yd);

            float sxd = control->GetTargetSize().x - control->GetSize().x;
            float syd = control->GetTargetSize().y - control->GetSize().y;

            float sdis = sqrt(sxd * sxd + syd * syd);
            
            //float dv = dif.length();

            if (dis < 3 && sdis <3)
            {
                control->SetTransist(false);
            }

        }

    }

    if (m_ControlPressed == nullptr) {
        for (auto control : list) {

            if (control->InBounds(mouse_pos))
            {

                if (m_ControlOver) {
                    if (m_ControlOver == control) {

                    }
                    else {
                        m_ControlOver->OnMouseLeave();
                        for (int i = 1; i < 3; i++) {
                            if (MouseB[i]) {
                                MouseB[i] = false;
                                m_ControlOver->OnMouseUp(i);
                            }
                        }
                        m_ControlOver = control;
                        control->OnMouseEnter();
                        prev_Click = 0;
                    }
                }
                else {
                    m_ControlOver = control;
                    control->OnMouseEnter();
                    prev_Click = 0;
                }
                break;
            }
        }

    }
    if (GameInput::Buttons[MOUSE_BUTTON_LEFT])
    {

        if (m_ControlOver != nullptr) {
            if (m_ControlPressed == nullptr) {
                m_ControlPressed = m_ControlOver;
                m_ControlPressed->OnMouseDown(0);
                m_ControlPressed->MousePressed(0);
               
                if (m_ControlPressed->IsWindow(false))
                {

                    auto win = m_ControlPressed->GetWindow();

                    if (win->GetIsDocked() == false) {
                        m_WindowSurface->RemoveChild(win);
                        m_WindowSurface->AddChild(win);
                    }
                    else {
                        if (win->GetRoot()->IsWindow(false)) {

                            auto nwin = win->GetRoot()->GetWindow();
                            if (nwin->GetIsDocked() == false) {

                                m_WindowSurface->RemoveChild(nwin);
                                m_WindowSurface->AddChild(nwin);
                            }


                        }
                       

                        
                    }
                }

                if (m_ControlActive != nullptr && m_ControlActive!=m_ControlOver) {
                    m_ControlActive->OnDeactivate();

                }
                else if (m_ControlActive == nullptr) {
                    m_ControlOver->OnActivate();
                }
                m_ControlActive = m_ControlOver;
              

                // Check if this is a window and being dragged by title area
            

                int ctime = clock();
                //prev_Click = clock();
                if (ctime < (prev_Click + 300) && m_FirstClick == m_ControlPressed) {
                    m_ControlPressed->OnMouseDoubleClick();
                    m_FirstClick = nullptr;
                }
                else {
                    m_FirstClick = nullptr;
                }
                prev_Click = ctime;
                m_FirstClick = m_ControlPressed;

            }
            else {

            }

        }
    }
    else {
        
        if (m_ControlPressed != nullptr) {
            std::cout << " Mouse Up GameUI" << std::endl;
            m_ControlPressed->OnMouseUp(0);
            m_ControlPressed->MouseUnPressed(0);
            m_ControlPressed = nullptr;
            m_DraggingWindow = nullptr; // Clear dragging window when mouse is released
        }
    }



    if (m_ControlPressed != nullptr) {
        IWindow* window = dynamic_cast<IWindow*>(m_ControlPressed);
        if (window != nullptr && window->GetCurrentArea() == AREA_TITLE) {
            if (window->BeingDragged()) {
                m_DraggingWindow = window;
            }
        }
    //    std::cout << "CP MOUSE MOVE:" << std::endl;
        m_ControlPressed->OnMouseMove(mouse_pos - m_ControlOver->GetRenderPosition(), GameInput::MouseDelta);
     //   m_ControlPressed->Move(GameInput::MouseDelta);
        m_ControlPressed->MouseWheelMoved(GameInput::MouseDelta.z);

    }
    else if (m_ControlOver != nullptr) {

      //  std::cout << "CO MOUSE MOVE" << std::endl;
        m_ControlOver->OnMouseMove(mouse_pos - m_ControlOver->GetRenderPosition(), GameInput::MouseDelta);
      //  m_ControlOver->Move(GameInput::MouseDelta);
        m_ControlOver->MouseWheelMoved(GameInput::MouseDelta.z);

    }

    m_RootControl->Update(delta);

    if (GetDraggingWindow() != nullptr) {

        auto beneath = GetBeneathWindow();
        if (beneath != nullptr) {

            IDocker* dock = dynamic_cast<IDocker*>(beneath);
            if (dock != nullptr) {
                std::cout << "WIN OVER" << std::endl;
                dock->WindowOver(GetDraggingWindow(), mouse_pos);
                m_DraggingDock = dock;
                m_DockingWindow = GetDraggingWindow();
                m_TabWindow = nullptr;
                m_TabTarget = nullptr;

            }
            else {
                if (beneath->IsWindow()) {
                    //exit(1);
                    std::cout << "BEN DRAG WIN" << std::endl;
                    m_TabWindow = beneath->GetWindow();
                    m_TabTarget = GetDraggingWindow();
                    

                    m_DraggingDock = nullptr;
                    m_DockingWindow = nullptr;
                }
                else {
                    m_TabWindow = nullptr;
                    m_TabTarget = nullptr;
                    m_DraggingDock = nullptr;
                    m_DockingWindow = nullptr;
                }
            }

            
        }
    
    }
    else {

        if (m_TabWindow != nullptr) {
            //exit(1);
         
            if (m_TabTarget == m_TabWindow) {
                m_TabWindow = nullptr;
                m_TabTarget = nullptr;
                return;
            }
            if (m_TabTarget->IsChild(m_TabWindow)) {

            }
            else {
                m_TabWindow->DockWindow(m_TabTarget);
                m_TabWindow = nullptr;
            }
        }
        if (m_DraggingDock != nullptr) {
         

            if (m_DraggingDock->HasNot(m_DockingWindow)) {

                auto check = m_DockingWindow->GetDock();
               // if (check != m_DraggingDock && check != nullptr) {

                    m_DraggingDock->DockWindow(m_DockingWindow, mouse_pos);
                    m_DraggingDock->WindowCancel();
  

            }
            m_DraggingDock = nullptr;
            //auto area = m_DraggingDock->GetDockAreaAtPosition(mouse_pos);

            //if (area != AREA_NONE) {
            //    m_DraggingDock->DockWindow(m_DockingWindow, area);
                //m_DraggingDock->WindowCancel();

            //}

        }

    }

    for (int i = 1; i < 3; i++) {

        if (GameInput::Buttons[i]) {

            if (MouseB[i] == false) {

                MouseB[i] = true;
                if (m_ControlOver != nullptr) {
                    m_ControlOver->OnMouseDown(i);
                }

            }

        }
        else {

            if (MouseB[i] == true) {

                MouseB[i] = false;
                if (m_ControlOver != nullptr) {

                    m_ControlOver->OnMouseUp(i);
                }

            }

        }

    }

    if (m_ControlOver) {

        m_ControlOver->OnMouseWheel(GameInput::MouseDelta.z);

    }

}
void GameUI::RenderUI()
{


  

	m_RootControl->PreRender();

    CurrentScissor = glm::vec4(0, 0, FutureApp::m_Inst->GetWidth(), FutureApp::m_Inst->GetHeight());
    UIHelp::Begin();
	m_RootControl->Render();
    
    if (m_ActiveMenu)
    {

        m_ActiveMenu->Render();

    }

    UIHelp::RemoveScissor();
    UIHelp::End();

}


IControl* GameUI::GetBeneathWindow()
{
    // If no window is being dragged, return nullptr
    if (m_DraggingWindow == nullptr) {
        return nullptr;
    }

    // Get current mouse position
    auto mouse_pos = GameInput::MousePosition;

    // Create a list of all controls to check against
    std::vector<IControl*> list;
    list = AddControls(list, m_RootControl);

    // Reverse the list to check front-to-back (same order as in UpdateUI)
    std::reverse(list.begin(), list.end());

    // To find control beneath dragged window, we need to skip the dragged window
    // and any of its children in the hit test
    bool skipWindow = false;

    for (auto control : list) {
        // Skip the dragged window and its children
        if (control == m_DraggingWindow) {
            skipWindow = true;
            continue;
        }

        // For child controls of the dragged window, continue skipping
        if (skipWindow) {
            IControl* parent = control->GetRoot();
            bool isChild = false;

            // Check if control is a child of the dragged window
            while (parent != nullptr) {
                if (parent == m_DraggingWindow) {
                    isChild = true;
                    break;
                }
                parent = parent->GetRoot();
            }

            if (isChild) {
                continue;
            }
        }

        // Check if control is under mouse position
        if (control->InBounds(mouse_pos)) {
            return control;
        }
    }

    // No control found beneath the window
    return nullptr;
}

void GameUI::ResetMouse() {

    m_ControlOver = nullptr;
    m_ControlPressed = nullptr;
    m_TabWindow = nullptr;
    m_TabTarget = nullptr;
    m_DraggingWindow = nullptr;
    GameInput::m_Dragging = nullptr;

}

void GameUI::SetUISize(int width,int height) {


    m_RootControl->Set(glm::vec2(0, 0), glm::vec2(width, height));

    m_RootControl->ApplyDockChildren();
    

}

void GameUI::InitForFPApp() {

}

void GameUI::InitForGame() {


}

void GameUI::SetDragWindow(IWindow* window) {


    m_ControlPressed = window;
    m_ControlOver = window;
    m_ControlPressed->OnMouseDown(0);
    if (m_ControlActive != nullptr) {
        m_ControlActive->OnDeactivate();
    }
    m_ControlActive = m_ControlOver;
    m_ControlOver->OnActivate();


    // Check if this is a window and being dragged by title area
    //IWindow* window = dynamic_cast<IWindow*>(m_ControlPressed);
    //if (window != nullptr && window->GetCurrentArea() == AREA_TITLE) {
        m_DraggingWindow = window;
    //}


    int ctime = clock();
    //prev_Click = clock();
    if (ctime < (prev_Click + 200) && m_FirstClick == m_ControlPressed) {
        m_ControlPressed->OnMouseDoubleClick();
        m_FirstClick = nullptr;
    }
    else {
        m_FirstClick = nullptr;
    }
    prev_Click = ctime;
    m_FirstClick = m_ControlPressed;
    //retur

    return;
    m_ControlPressed = window;
    m_ControlOver = window;
    m_DraggingWindow = nullptr;
    GameInput::m_Dragging = window;
    

}

void GameUI::SetMainMenu(IMainMenu* menu)
{
    if (m_ActiveMenu != nullptr) {
        m_RootControl->RemoveChild(m_ActiveMenu);
    }
    if (menu == nullptr) {
        menu = m_DefaultMenu;
    }
    m_ActiveMenu = menu;
    m_RootControl->AddChild(m_ActiveMenu);

}

void GameUI::Deactivate() {

    if (m_ControlActive != nullptr) {
        m_ControlActive->OnDeactivate();
        m_ControlActive = nullptr;
    }

}

void GameUI::ClearMenus() {

    m_ActiveMenu->Clear();
    m_ActiveMenu->ClearChildren();
    

}