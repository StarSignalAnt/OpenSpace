#include "IWindow.h"
#include "UIHelp.h"
#include "GameInput.h"
#include "FutureApp.h"
#include "IControlGroup.h"
#include "IVerticalScroller.h"
#include "IHorizontalScroller.h"
#include <iostream>
#include "Texture2D.h"
#include "IDocker.h"
#include "GameUI.h"
#include "IMainMenu.h"
#include "UITheme.h"
#include "IToolBar.h"
#include <list>

void IWindow::Update(float delta)
{
	if (GetRoot() != GameUI::m_Inst->GetWindowSurface()) {
	//	GetRoot()->RemoveChild(this);
//		GameUI::m_Inst->GetWindowSurface()->AddChild(this);


	}
	UpdateChildren(delta);
}

void IWindow::Render()
{
	auto pos = GetRenderPosition();

	m_TitleBG->Grab(pos.x, pos.y);
	m_ClientBG->Grab(pos.x,pos.y + 20);

	auto theme = GameUI::GetTheme();


	
	if (theme->GetWindowBlur()) {
	//	UIHelp::DrawImageBlur(pos + glm::vec2(0, 20 + (m_Size.y - 20)), glm::vec2(m_Size.x, -(m_Size.y - 21)), m_ClientBG, glm::vec4(1, 1, 1, 1), 2.1);
	}

	auto bg = theme->GetBackground();
	bg.a = theme->GetWindowAlpha();

	UIHelp::DrawRect(pos + glm::vec2(0, 20), m_Size + glm::vec2(0, -17), theme->GetWindowDark());
	UIHelp::DrawRect(pos + glm::vec2(3, 20), m_Size + glm::vec2(-6, -21), theme->GetWindowLight());
	 UIHelp::DrawRect(pos + glm::vec2(0,0),  glm::vec2(m_Size.x, 20), theme->GetWindowTileBar());
//	UIHelp::DrawRect(pos, glm::vec2(m_Size.x, 20), glm::vec4(1,1,1, 1.0f));
// 
// 
// 
	//UIHelp::DrawImage(pos, glm::vec2(m_Size.x, 25),m_TitleBarImage, glm::vec4(0.678*1.8, 0.847*1.8, 0.902*1.8, 1));
	//return;

	std::vector<std::string> tabs;

	tabs.push_back(m_Text);
	for (auto docked : m_DockedWindows) {

		tabs.push_back(docked->GetText());

	}


	int sx = 0;

	int idx = 0;

	for (auto title : tabs) {



		int tab_width = UIHelp::StrWidth(title) + 60;

	//	UIHelp::DrawImageBlur(pos + glm::vec2(sx, 20), glm::vec2(tab_width + 10 , -20), m_TitleBG, glm::vec4(1, 1, 1, 1), 1.3);

		if (idx == m_CurrentTab) {

		//	UIHelp::DrawImageWithBG(pos + glm::vec2(sx+1, 0), glm::vec2(tab_width + 10, 19), glm::vec4(0.678 * 0.6, 0.847 * 0.6, 0.902 * 0.6, 0.8f));
		//	UIHelp::DrawOutlineRect(pos + glm::vec2(sx, 0), glm::vec2(tab_width + 10, 19), glm::vec4(0.678 * 0.8, 0.847 * 0.8, 0.902 * 0.8, 0.8f));
			UIHelp::DrawRect(pos + glm::vec2(sx+1, 3), glm::vec2(tab_width + 10-2, 20-3),theme->GetWindowLight());



			UIHelp::DrawText(pos + glm::vec2(sx + 5, 10 - UIHelp::StrHeight(title) / 2), title, glm::vec4(1, 1, 1, 1));
			UIHelp::DrawText(pos + glm::vec2(sx + 1 + tab_width-6, 8), "X", glm::vec4(1, 1, 1, 1),0.8);
			m_ClosePos = glm::vec2(sx + 1 + tab_width - 6, 8);

		}
		else {
//			UIHelp::DrawImageWithBG(pos + glm::vec2(sx, 0), glm::vec2(tab_width + 10, 20), glm::vec4(0.678 * 0.1, 0.847 * 0.1, 0.902 * 0.1, 0.8f));
		//	UIHelp::DrawRect(pos + glm::vec2(sx, 4), glm::vec2(tab_width + 10, 14), glm::vec4(0.678 * 0.8, 0.847 * 0.8, 0.902 * 0.8, 0.8f));

			UIHelp::DrawRect(pos + glm::vec2(sx, 4), glm::vec2(tab_width + 10, 14),theme->GetWindowDark());

			UIHelp::DrawText(pos + glm::vec2(sx + 5, 12 - UIHelp::StrHeight(title) / 2), title, glm::vec4(1, 1, 1, 1));
		}


		//UIHelp::DrawText(pos + glm::vec2(sx + 5,10- UIHelp::StrHeight(title) / 2) , title, glm::vec4(1, 1, 1, 1));

		auto tpos = pos;
		//pos.x = tpos.x + 5;
		//pos.y += 20 / 2;
		//	pos.x -= UIHelp::StrWidth(m_Text) / 2;
		//pos.y -= UIHelp::StrHeight(m_Text) / 2;

		//UIHelp::DrawText(pos + glm::vec2(3, 3), m_Text, glm::vec4(0, 0, 0, 1));
		//UIHelp::DrawText(pos, m_Text, glm::vec4(1, 1, 1, 1));
		sx = sx + tab_width + 10;
		idx++;

	}

	//UIHelp::DrawText(pos + glm::vec2(m_Size.x - 15, m_Size.y - 15),"X", glm::vec4(1, 1, 1, 1));
//	UIHelp::DrawText(pos, m_Text, glm::vec4(1, 1, 1, 1));

	//UIHelp::DrawOutlineRect(pos + glm::vec2(0, 19), m_Size + glm::vec2(0, -19), theme->GetForeground());
	
	pos += glm::vec2(0, 19);
	auto size = m_Size + glm::vec2(0, -19);
	auto color = m_Color;
//	UIHelp::DrawRect(pos, glm::vec2(1, size.y), color);
//	UIHelp::DrawRect(pos + glm::vec2(0, size.y), glm::vec2(size.x, 1), color);
//	UIHelp::DrawRect(pos + glm::vec2(size.x, 0), glm::vec2(1, size.y), color);


	RenderChildren();
}

void IWindow::OnMouseDown(int button)
{


	if (m_CurrentArea == AREA_TITLE || m_CurrentArea == AREA_RESIZER || m_CurrentArea == AREA_CLOSE || m_CurrentArea == AREA_LEFT || m_CurrentArea == AREA_BOTTOM || m_CurrentArea == AREA_RIGHT) {

		m_Locked = true;
		if (m_CurrentArea == AREA_CLOSE) {
			Close();
			return;
		}
			
	}

	

	m_MouseIn = true;
	if (m_ActiveMenu != nullptr) {
//		m_ActiveMenu->SetAppTitle(m_AppTitle);
	}
	GameUI::m_Inst->SetMainMenu(m_ActiveMenu);

	auto pos = GetRenderPosition();
	std::vector<std::string> tabs;

	tabs.push_back(m_Text);
	for (auto docked : m_DockedWindows) {

		tabs.push_back(docked->GetText());

	}

	m_MousePos = pos + m_MousePos;

	int sx = 0;

	int idx = 0;

	if (tabs.size() == 1)
	{
		m_Dragging = true;
		GameInput::m_Dragging = this;
		return;
	}
	for (auto title : tabs) {



		int tab_width = UIHelp::StrWidth(title) + 60;

		//	UIHelp::DrawImageBlur(pos + glm::vec2(sx, 20), glm::vec2(tab_width + 10 , -20), m_TitleBG, glm::vec4(1, 1, 1, 1), 1.3);

		if (idx == m_CurrentTab) {
		//	UIHelp::DrawRect(pos + glm::vec2(sx, 0), glm::vec2(tab_width + 10, 20), glm::vec4(0.678 * 0.1, 0.847 * 0.1, 0.902 * 0.1, 0.8f));
		}
		else {
	//		UIHelp::DrawRect(pos + glm::vec2(sx, 0), glm::vec2(tab_width + 10, 20), glm::vec4(0.678 * 0.35, 0.847 * 0.35, 0.902 * 0.35, 0.8f));
		}
		int tx = pos.x + sx;
		int ex = pos.x + sx + tab_width + 10;


		int ay = 0;

		if (idx == m_CurrentTab) {

			ay = 0;

		}
		else {
			ay = 4;
		}

		if (m_MousePos.x >= tx && m_MousePos.x <ex)
		{
		//	if (m_MousePos.y >= pos.y+ay && m_MousePos.y <= pos.y + (20-ay))
			{
				
					m_CurrentTab = idx;
					if (idx > 0) {
						std::cout << "Changed tab:" << idx << std::endl;
						RemoveChild(m_ClientArea);
						auto client = m_DockedWindows[m_CurrentTab - 1]->GetClientArea();
						m_ClientArea = client;
						AddChild(m_ClientArea);
						GameUI::m_Inst->SetUISize(FutureApp::m_Inst->GetWidth(), FutureApp::m_Inst->GetHeight());
					}
					else {
					
						RemoveChild(m_ClientArea);
						AddChild(m_BaseArea);
						m_ClientArea = m_BaseArea;
						GameUI::m_Inst->SetUISize(FutureApp::m_Inst->GetWidth(), FutureApp::m_Inst->GetHeight());

					}


					



			}
		}


		//UIHelp::DrawText(pos + glm::vec2(sx + 5, 10 - UIHelp::StrHeight(title) / 2), title, glm::vec4(1, 1, 1, 1));

		auto tpos = pos;
		//pos.x = tpos.x + 5;
		//pos.y += 20 / 2;
		//	pos.x -= UIHelp::StrWidth(m_Text) / 2;
		//pos.y -= UIHelp::StrHeight(m_Text) / 2;

		//UIHelp::DrawText(pos + glm::vec2(3, 3), m_Text, glm::vec4(0, 0, 0, 1));
		//UIHelp::DrawText(pos, m_Text, glm::vec4(1, 1, 1, 1));
		sx = sx + tab_width + 10;
		idx++;

	}

	m_Dragging = true;
	GameInput::m_Dragging = this;

}
void IWindow::OnMouseUp(int button)
{
	m_Dragging = false;
	GameInput::m_Dragging = nullptr;
	m_MouseIn = false;
	std::cout << "Mouse Up:" << m_Text << std::endl;
	m_Locked = false;
}

void IWindow::OnMouseDoubleClick()
{


	if (m_CurrentArea == AREA_TITLE) {
		if (m_Size.x < FutureApp::m_Inst->GetWidth() || m_Size.y < FutureApp::m_Inst->GetHeight() || m_Position.x>0 || m_Position.y>0)
		{
			Set(glm::vec2(0, 30), glm::vec2(FutureApp::m_Inst->GetWidth(), FutureApp::m_Inst->GetHeight()-60));
		
		}
	}
}

void IWindow::OnMouseMove(glm::vec2 position, glm::vec2 delta)
{



	m_MousePos = position;

	if (!m_Locked) {
		if (m_MousePos.y < 20 && m_MousePos.x < m_Size.x)
		{
			m_CurrentArea = AREA_TITLE;
		}

		if (m_MousePos.x >= 0 && m_MousePos.x < 4 && m_MousePos.y>20 && m_MousePos.y <= m_Size.y) {
			m_CurrentArea = AREA_LEFT;
		}
		if (m_MousePos.y > m_Size.y - 12 && m_MousePos.y <= m_Size.y) {
			m_CurrentArea = AREA_BOTTOM;
		}
		if (m_MousePos.x > m_Size.x - 8 && m_MousePos.x <= m_Size.x)
		{
			if (m_MousePos.y > 20)
			{
				m_CurrentArea = AREA_RIGHT;
			}

		}
		if (m_MousePos.x > m_Size.x - 12 && m_MousePos.y > m_Size.y - 12 && m_MousePos.x < m_Size.x && m_MousePos.y < m_Size.y)
		{
			m_CurrentArea = AREA_RESIZER;
		}
		if (m_MousePos.x > m_ClosePos.x && m_MousePos.x<m_ClosePos.x + 8 && m_MousePos.y > m_ClosePos.y && m_MousePos.y < m_ClosePos.y + 8)
		{
			m_CurrentArea = AREA_CLOSE;

		}
	}


	if (m_Dragging) {

		if (m_CurrentTab == 0) {
			m_MousePos = position;
			if (delta.x != 0 || delta.y != 0)
			{
				if (m_Docked) {
					if (m_CurrentArea == AREA_TITLE) {
						if (m_Dock != nullptr) {
							m_Dock->UndockWindow(this);
							m_Dock = nullptr;
							m_Docked = false;
							m_IsDocked = false;
							return;
						}
					}
				}
			
				if (m_Locked) {
					switch (m_CurrentArea) {
					case AREA_TITLE:
						//if (m_CurrentArea == AREA_TITLE) {
						m_Position += delta;
						//}
						break;
					case AREA_RESIZER:


						if (!m_IsDocked) {
							auto new_size = m_Size += delta;
							if (new_size.x < 128) new_size.x = 128;
							if (new_size.y < 128) new_size.y = 128;
							Set(GetPosition(), new_size);
						}

						break;
					case AREA_LEFT:
					{
						if (!m_IsDocked) {
							if (delta.x < 0) {
								m_Position = m_Position + glm::vec2(delta.x, 0);
								auto new_size = m_Size;
								new_size.x += -delta.x;
								if (new_size.x < 128) new_size.x = 128;
								if (new_size.y < 128) new_size.y = 128;
								Set(m_Position, new_size);
							}
							else {
								m_Position = m_Position + glm::vec2(delta.x, 0);
								auto new_size = m_Size;
								new_size.x -= delta.x;
								if (new_size.x < 128) new_size.x = 128;
								if (new_size.y < 128) new_size.y = 128;
								Set(m_Position, new_size);
							}
						}
					}
						break;
					case AREA_BOTTOM:

						if (!m_IsDocked) {
							if (delta.y < 0) {

								auto new_size = m_Size;
								new_size.y += delta.y;
								if (new_size.x < 128) new_size.x = 128;
								if (new_size.y < 128) new_size.y = 128;
								Set(m_Position, new_size);

							}
							else {

								auto new_size = m_Size;
								new_size.y += delta.y;

								Set(m_Position, new_size);

							}
						}
						break;
					case AREA_RIGHT:
						if (!m_IsDocked) {
							if (delta.x < 0) {
								auto new_size = m_Size;
								new_size.x += delta.x;
								if (new_size.x < 128) new_size.x = 128;
								Set(m_Position, new_size);
							}
							else {
								auto new_size = m_Size;
								new_size.x += delta.x;
								Set(m_Position, new_size);
							}

						}
						break;
					}
				}
			}
		}
	}
	//return;

	if (m_CurrentArea == AREA_TITLE) {
		if (m_Dragging) {
			
			/*
			if (GetRoot()->InBounds(position+GetRenderPosition())) {

				if (m_Outside) {
					if (GetRenderPosition().x+m_MousePos.x < GetRoot()->GetSize().x / 2)
					{
						m_Position = (GetRenderPosition() + position) - glm::vec2(45, 25);
					}
					else {
						m_Position = (GetRenderPosition() + position) - glm::vec2(45, 25);

					}
				}
				else {
				

					if (m_Position.x < -50.0f) {
						m_Position.x = -50;
					}
					if (m_Position.y < -50) {
						m_Position.y = -50;
					}
					if (m_Position.x > (GetRoot()->GetSize().x - 5)) {
						m_Position.x = GetRoot()->GetSize().x - 5;
					}
					if (m_Position.y > (GetRoot()->GetSize().y - 5))
					{
						m_Position.y = GetRoot()->GetSize().y - 5;
					}
				}
				m_Outside = false;
			}
			else {
				m_Outside = true;
			}

			*/

			if (delta.x < -4 || delta.x>4 || delta.y < -4 || delta.y>4)
			{

				if (m_CurrentTab != 0) {


					
					std::cout << "Removing Tab" << std::endl;
					//return;
				//	m_MousePos -= delta;
					RemoveChild(m_ClientArea);
					m_ClientArea->SetRoot(m_DockedWindows[m_CurrentTab - 1]);
					AddChild(m_BaseArea);
					m_ClientArea = m_BaseArea;
					AlignWindow();
					
					
					auto win = m_DockedWindows[m_CurrentTab - 1];
					win->GetRoot()->RemoveChild(win);

					//this->GetRoot()->AddChild(m_DockedWindows[m_CurrentTab - 1]);
					GameUI::m_Inst->GetWindowSurface()->AddChild(m_DockedWindows[m_CurrentTab - 1]);
					m_DockedWindows[m_CurrentTab - 1]->AlignWindow();
					m_DockedWindows.erase(m_DockedWindows.begin() + m_CurrentTab-1);
					m_CurrentTab = 0;
					//GameUI::m_Inst->ResetMouse();
					//GameInput::m_Dragging = nullptr;
				
					m_Dragging = false;
					win->Set(GetRenderPosition()+ glm::vec2(m_MousePos.x-25, m_MousePos.y-8));
					GameUI::m_Inst->SetDragWindow(win);
					//win->OnMouseMove(glm::vec2(30, 10),glm::vec2(0,0));
					//win->OnMouseDown(0);



					//GameUI::m_Inst->ResetMouse();

					m_Dragging = false;

				}
				else {
					
				}
			}
		}
	}
	if (m_CurrentArea == AREA_RESIZER)
	{
		
	}
}

bool IWindow::InBounds(glm::vec2 position)
{
	return IControl::InBounds(position);
	int tab_width = UIHelp::StrWidth(m_Text)+10;

	glm::vec2 root = GetRenderPosition();
	if (position.x > root.x && position.x < root.x + m_Size.x &&
		position.y > root.y && position.y < root.y + 16)
	{
	//	m_CurrentArea = AREA_TITLE;
		return true;
	}

	if (position.x > root.x + m_Size.x - 16 && position.x<root.x + m_Size.x &&
		position.y>root.y + m_Size.y - 16 && position.y < root.y + m_Size.y)
	{
	//	m_CurrentArea = AREA_RESIZER;
		return true;
	}

//	m_CurrentArea = AREA_CLIENT;
	
	if (position.x > root.x & position.x < root.x + m_Size.x - 18)
	{
		if (position.y > root.y && position.y < root.y + m_Size.y - 18) {
			return true;
		}
	}
	return false;
	//return IControl::InBounds(position);


	return false;
}

void IWindow::InitWindow() {

	if (m_Buttons) {
		m_CloseButton = new IButton("X", glm::vec2(m_Size.x - 16, 2), glm::vec2(16, 16));
		m_MaximizeButton = new IButton("E", glm::vec2(m_Size.x - 32, 2), glm::vec2(16, 16));
		m_MinimizeButton = new IButton("_", glm::vec2(m_Size.x - 48, 2), glm::vec2(16, 16));
	}

	
	
	m_YScroller = new IVerticalScroller(glm::vec2(m_Size.x - 10, 21), glm::vec2(10, m_Size.y - 36));
	m_XScroller = new IHorizontalScroller(glm::vec2(0, m_Size.y - 10), glm::vec2(m_Size.x - 13, 10));
	m_Resizer = new IButton(".", glm::vec2(m_Size.x-10,m_Size.y-10),glm::vec2(10,10));
	//m_CloseButton->SetRenderBody(false);


	m_TitleBG = new Texture2D(m_Size.x, 20);
	m_ClientBG = new Texture2D(m_Size.x, m_Size.y - 20);

//	m_MaximizeButton->SetRenderBody(false);
//	m_MinimizeButton->SetRenderBody(false);
	if (m_Buttons) {
		AddChild(m_CloseButton);
		AddChild(m_MaximizeButton);
		AddChild(m_MinimizeButton);
	}

	if (m_HasToolBar) {
		m_ClientArea = new IControlGroup(glm::vec2(1, 66), glm::vec2(m_Size.x - 12, m_Size.y - 67));
		m_ToolBar = new IToolBar;
		m_ToolBar->Set(glm::vec2(0, 26), glm::vec2(m_Size.x, 30));

		AddChild(m_ToolBar);

	}
	else {
		m_ClientArea = new IControlGroup(glm::vec2(1, 26), glm::vec2(m_Size.x - 12, m_Size.y - 32));
	}
	AddChild(m_ClientArea);
	AddChild(m_YScroller);
	AddChild(m_XScroller);
//	AddChild(m_Resizer);
	m_BaseArea = m_ClientArea;

	m_Resizer->SetOnMoved([&](glm::vec2 delta) {

		//if (m_Dragging) {
			m_Size += delta;
			if (m_Size.x < 220)
			{

				float v = m_Size.x - 220;
				m_Size.x = 220;
				m_Position.x += v;
			}
			if (m_Size.y < 128) {
				float v = m_Size.y - 128;
				m_Position.y += v;
				m_Size.y = 128;
			}
			AlignWindow();
		//}

		});

	

	m_ClientArea->SetCullChildren(true);
	m_YScroller->SetOnScrolled([&](float y) {

		m_ClientArea->SetOffset(glm::vec2(m_ClientArea->GetOffset().x, -y));

		});

	m_XScroller->SetOnScrolled([&](float x) {

		m_ClientArea->SetOffset(glm::vec2(-x, m_ClientArea->GetOffset().y));

		});

	//m_ClientArea->SetOffset(glm::vec2(0, 200));


	if (m_Buttons) {
		m_MinimizeButton->SetOnClick([&](void* data) {
			if (m_OriginalSize.x > 0 && m_OriginalSize.y > 0) {
				m_Position = m_OriginalPosition;
				m_Size = m_OriginalSize;
				AlignWindow();
			}
			//.m_Size = glm::vec2(m_Size.x, 20);
			});

		m_MaximizeButton->SetOnClick([&](void* data) {


			auto pos = GetRenderPosition();
			if (pos.x > 0 || pos.y > 0 || m_Size.x < FutureApp::m_Inst->GetWidth() || m_Size.y < FutureApp::m_Inst->GetHeight())
			{
				m_OriginalPosition = m_Position;
				m_OriginalSize = m_Size;
				m_Position = glm::vec2(0, 0);
				m_Size = glm::vec2(FutureApp::m_Inst->GetWidth(), FutureApp::m_Inst->GetHeight());
				AlignWindow();
			}

			//		//if(m_Position.x>0 || m_Position.y )

			});

		m_CloseButton->SetOnClick([&](void* dat) {

			m_RootControl->RemoveChild(this);

			});

	}

	m_TitleBarImage = new Texture2D("engine/ui/windowTitle.png");
	AlignWindow();
	Setup();
}

void IWindow::AlignWindow() {

	if (m_Buttons) {
		m_CloseButton->Set(glm::vec2(m_Size.x - 16, 2));
		m_MaximizeButton->Set(glm::vec2(m_Size.x - 32, 2));
		m_MinimizeButton->Set(glm::vec2(m_Size.x - 48, 2));
	}

	if (m_HasToolBar) {
		m_ClientArea->Set(glm::vec2(5, 66), glm::vec2(m_Size.x - 17, m_Size.y - 75));
	}
	else {
		m_ClientArea->Set(glm::vec2(5, 26), glm::vec2(m_Size.x - 22, m_Size.y - 40));
	}
	std::list<IControlGroup*> groups;

	groups.push_back(m_BaseArea);

	for (auto win : m_DockedWindows) {

		groups.push_back(win->GetClientArea());

	}

	for (auto g : groups) {

		if (g != m_ClientArea) {
			if (m_HasToolBar) {
				g->Set(glm::vec2(1, 56), glm::vec2(m_Size.x - 12, m_Size.y - 57));
			}
			else {
				g->Set(glm::vec2(5, 26), glm::vec2(m_Size.x - 22, m_Size.y - 27));
			}
		}
	}

	RemoveChild(m_YScroller);
	m_TitleBG->Free();

	m_TitleBG = new Texture2D(m_Size.x, 20);
	m_ClientBG->Free();
	m_ClientBG = new Texture2D(m_Size.x, m_Size.y - 20);

	m_Resizer->Set(glm::vec2(m_Size.x - 8, m_Size.y - 8), glm::vec2(7, 7));


	auto sv = m_YScroller->GetScrollPosition();
	m_YScroller = new IVerticalScroller(glm::vec2(m_Size.x - 18, 21), glm::vec2(10, m_Size.y - 31));
	
	AddChild(m_YScroller);
	int m_Height = m_ClientArea->GetMaxHeight();

	m_YScroller->SetContentHeight(m_Height);


	m_YScroller->SetOnScrolled([&](float y) {

		m_ClientArea->SetOffset(glm::vec2(m_ClientArea->GetOffset().x, -y));

		});


	m_YScroller->SetScrollPosition(sv);

	RemoveChild(m_XScroller);
	auto sh = m_XScroller->GetScrollPosition();
	m_XScroller = new IHorizontalScroller(glm::vec2(0, m_Size.y - 18), glm::vec2(m_Size.x - 13, 10));

	AddChild(m_XScroller);
	int m_Width = m_ClientArea->GetMaxWidth();

	m_XScroller->SetContentWidth(m_Width);


	m_XScroller->SetOnScrolled([&](float x) {

		m_ClientArea->SetOffset(glm::vec2(-x, m_ClientArea->GetOffset().y));

		});

	m_XScroller->SetScrollPosition(sh);
	//m_YScroller->SetScrollPosition(0);
	//float sy = -m_ClientArea->GetOffset().y;
	
	

	//std::cout << "OS:" << m_ClientArea->GetOffset().y << std::endl;

	//if(m_ClientArea->GetOffset().

	if (m_HasToolBar) {
		m_ToolBar->Set(glm::vec2(0, 26), glm::vec2(m_Size.x, 33));
		//m_ToolBar->SetDockType(DockType::m_Up);

	}

}

void IWindow::AddClientControl(IControl* control) {

	m_ClientArea->AddChild(control);
	
	int m_Height = m_ClientArea->GetMaxHeight();

	m_YScroller->SetContentHeight(m_Height);
	m_XScroller->SetContentWidth(m_ClientArea->GetMaxWidth());

}

void IWindow::AfterSet() {

	AlignWindow();

	//m_ClientArea->ApplyDockChildren();

	

	

}

void IWindow::DockWindow(IWindow* window) {

	m_DockedWindows.push_back(window);
	window->GetRoot()->RemoveChild(window);
	AlignWindow();
	
//	exit(2);

}