#pragma once
#include "FutureState.h"
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include "SoundLib.h"
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
	std::vector<Texture2D*> m_Images;
	int m_CurrentImg = 0;
	float m_Alpha = 0.0f;
	int state = 0;
	int time = 0;
	std::shared_ptr<Channel> m_Music;

};

