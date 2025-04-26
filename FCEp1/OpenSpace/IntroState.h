#pragma once
#include "FutureState.h"
class IntroState :
    public FutureState
{
public:


	void InitState() override;
	void UpdateState(float delta) override;
	void RenderState() override;
	void EndState() override;


};

