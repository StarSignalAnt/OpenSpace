#pragma once
#include "IControl.h"
class IControlGroup :
    public IControl
{
public:

	IControlGroup();
	IControlGroup(glm::vec2 position, glm::vec2 size);
	void Update(float delta) override;
	void Render() override;
	

};

