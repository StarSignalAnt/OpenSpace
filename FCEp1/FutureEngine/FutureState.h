#pragma once
class FutureState
{
public:

	virtual void InitState() = 0;
	virtual void UpdateState(float delta) = 0;
	virtual void RenderState() = 0;
	virtual void EndState() = 0;

private:
};

