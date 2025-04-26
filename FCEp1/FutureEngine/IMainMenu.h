#pragma once
#include "IControl.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <functional>

class IVerticalMenu;

struct MenuItem {

	std::string m_Name;
	std::vector<MenuItem*> m_Items;
	MenuItem(std::string name) {
		m_Name = name;

	}
	void AddItem(MenuItem* item) {
		m_Items.push_back(item);
	}
	bool m_Open = false;
	IVerticalMenu* m_ActiveControl = nullptr;
	int m_DrawX, m_DrawY;
	std::function<void()> OnClick = nullptr;
	int m_Extra = 0;
	std::function<void(MenuItem*)> OnSelect = nullptr;
	void* m_Data;
};

class IMainMenu : public IControl
{
public:

	IMainMenu();
	void Update(float delta) override;
	void Render() override;
	void AddItem(MenuItem* item) {
		m_Items.push_back(item);
	}
	void OnMouseMove(glm::vec2 position, glm::vec2 delta) override;
	void OnMouseDown(int button) override;
	void SetAppTitle(std::string title) { m_AppTitle = title; }
	std::string GetAppTitle() {
		return m_AppTitle;
	}

	
	void Clear()
	{
	
		m_OverItem = nullptr;
		if (m_OpenItem != nullptr) {
			m_OpenItem->m_Open = false;
		}
		m_OpenItem = nullptr;
		ClearChildren();
	}

private:

	std::vector<MenuItem*> m_Items;
	MenuItem* m_OverItem = nullptr;
	MenuItem* m_OpenItem = nullptr;
	std::string m_AppTitle = "";

};

