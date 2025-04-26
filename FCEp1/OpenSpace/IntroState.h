#pragma once
#include "FutureState.h"

class SmartDraw;
class Texture2D;

class IntroState :
    public FutureState
{
public:


	void InitState() override;
	void UpdateState(float delta) override;
	void RenderState() override;
	void EndState() override;

private:

	SmartDraw* m_Draw;
	Texture2D* m_Title;

};

