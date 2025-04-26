#pragma once
#include "IControl.h"
#include <vector>
#include <string>


struct MenuItem;

class IVerticalMenu :
    public IControl
{
public:

	void Update(float delta) override;
	void Render() override;
	void AddItem(MenuItem* item) {
		m_Items.push_back(item);
	}
	void OnMouseMove(glm::vec2 position, glm::vec2 delta) override;
	void OnMouseDown(int button) override;

private:

    std::vector<MenuItem*> m_Items;
    MenuItem* m_OverItem = nullptr;
	MenuItem* m_OpenItem = nullptr;
};

