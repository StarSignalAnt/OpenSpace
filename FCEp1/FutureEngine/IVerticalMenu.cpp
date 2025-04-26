#include "IVerticalMenu.h"
#include "UIHelp.h"
#include "IMainMenu.h"
#include "UITheme.h"
#include "GameUI.h"

void IVerticalMenu::OnMouseMove(glm::vec2 position, glm::vec2 delta) {

	int dy = 10;
	int dx = 25;
	m_OverItem = nullptr;
	for (auto item : m_Items) {

//		UIHelp::DrawText(GetRenderPosition() + glm::vec2(dx, dy), item->m_Name, glm::vec4(1, 1, 1, 1));

		if (position.y >= dy-5 && position.y <= dy-10 + 25)
		{
			m_OverItem = item;
		}

		dy = dy + 25;

	}

}


void IVerticalMenu::OnMouseDown(int button) {

	if (m_OverItem) {

		if (m_OverItem->m_Items.size() == 0) {
			if (m_OverItem->OnClick)
			{
				m_OverItem->OnClick();
	
			}
			if (m_OverItem->OnSelect) {
				m_OverItem->OnSelect(m_OverItem);
			}
			GetRoot()->RemoveChild(this);
			GameUI::m_Inst->ClearMenus();


		}
		else {

			if (m_OverItem->m_Open == false)
			{

				if (m_OpenItem != nullptr) {

					RemoveChild(m_OpenItem->m_ActiveControl);
					delete m_OpenItem->m_ActiveControl;
					m_OpenItem->m_Open = false;

				}

				IVerticalMenu* new_menu = new IVerticalMenu;

				int height = m_OverItem->m_Items.size() * 25;

				int big_width = 0;

				for (auto item : m_OverItem->m_Items)
				{

					int string_w = UIHelp::StrWidth(item->m_Name);
					if (string_w > big_width) big_width = string_w;
					new_menu->AddItem(item);
					item->m_Open = false;
					item->m_ActiveControl = nullptr;
				}



				new_menu->Set(glm::vec2(m_Size.x +1, m_OverItem->m_DrawY-10), glm::vec2(big_width + 60, height + 5));

				AddChild(new_menu);
				new_menu->SetIgnoreScissor(true);

				m_OverItem->m_ActiveControl = new_menu;

				m_OverItem->m_Open = true;

				m_OpenItem = m_OverItem;
			}
			else {

				RemoveChild(m_OverItem->m_ActiveControl);
				delete m_OverItem->m_ActiveControl;
				m_OverItem->m_Open = false;
				if (m_OpenItem == m_OverItem) {
					m_OpenItem = nullptr;
				}


			}


		}

	}
}

void IVerticalMenu::Update(float delta) {


}

void IVerticalMenu::Render() {


//	UIHelp::DrawImageWithBG(GetRenderPosition()+glm::vec2(2,2), GetSize()+glm::vec2(-3,-3), glm::vec4(1.5,1.5,1.5, 0.88));
	//UIHelp::DrawOutlineRect(GetRenderPosition(), GetSize(), glm::vec4(1, 1, 1, 0.85));
	
	SetIgnoreScissor(true);

	auto theme = GameUI::GetTheme();

	UIHelp::DrawRect(GetRenderPosition()+glm::vec2(1,1), GetSize()+glm::vec2(-2,-2),theme->GetWindowLight());

	int dy = 10;
	int dx = 25;
	for (auto item : m_Items) {


		if (item == m_OverItem) {
		//	UIHelp::DrawOutlineRect(GetRenderPosition() + glm::vec2(2, dy - 6), glm::vec2(GetSize().x-4, 22), glm::vec4(0.8f, 0.8f, 0.8f, 0.5f));
		//	UIHelp::DrawImageWithBG(GetRenderPosition() + glm::vec2(3, dy - 5), glm::vec2(GetSize().x - 6, 22 - 2), glm::vec4(0.55f, 0.55f, 0.55f, 0.5f));
			UIHelp::DrawRect(GetRenderPosition() + glm::vec2(3, dy-5), glm::vec2(GetSize().x-6, 22-2), glm::vec4(0.25f, 0.25f, 0.25f, 0.85f));
		//	UIHelp::DrawImageWithBG(GetRenderPosition() + glm::vec2(3, dy - 5), glm::vec2(GetSize().x - 6, 22 - 2), glm::vec4(0.55f, 0.55f, 0.55f, 0.5f));
		}

		UIHelp::DrawText(GetRenderPosition()+glm::vec2(dx, dy), item->m_Name, glm::vec4(1, 1, 1, 1));

		item->m_DrawY = dy;
		dy = dy + 25;

	}

	RenderChildren();

}