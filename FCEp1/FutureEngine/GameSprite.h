#pragma once
#include "GameObj.h"
#include <vector>
#include <string>

class GameAnimation;

class GameSprite : public GameObj
{
public:

	GameSprite();
	void AddAnimation(GameAnimation* anim);
	void SetAnimation(std::string name);
	struct AnimFrame* GetCurrentFrame();
	void PlayAnimation(std::string name) { SetAnimation(name); }
	GameAnimation* GetCurrentAnimation() { return m_CurrentAnimation; }

	void Set(bool castShadows, bool recievesShadows, bool recievesLight)
	{
		m_CastShadow = castShadows;
		m_ReceivesShadow = recievesShadows;
		m_ReceivesLight = recievesLight;
	}
	bool GetCastShadows() { return m_CastShadow; }
	bool GetReceivesShadows() { return m_ReceivesShadow; }
	bool GetReceivesLight() { return m_ReceivesLight; }

private:

	std::vector<GameAnimation*> m_Animations;
	GameAnimation* m_CurrentAnimation;
	bool m_CastShadow = true;
	bool m_ReceivesShadow = false;
	bool m_ReceivesLight = true;

};

