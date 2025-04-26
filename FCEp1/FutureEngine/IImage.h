#pragma once
#include "IControl.h"
class IImage :
    public IControl
{
public:

	IImage() : IControl() {}
	IImage(glm::vec2 position, glm::vec2 size) : IControl(position, size) {}
	void Update(float delta) override;
	void Render() override;


};

