#pragma once
#include <FutureState.h>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include "SoundLib.h"
class GameFont;
class Starfield;
class Texture2D;
class GameUI;

class MenuState :
    public FutureState
{
public:

	void InitState() override;
	void UpdateState(float delta) override;
	void RenderState() override;
	void EndState() override;

private:

	Starfield* m_Stars;
	Texture2D* m_Title;
	GameFont* m_Font;
	GameUI* m_UI;
	std::shared_ptr<Channel> m_Music;
	Texture2D* m_Logo;
};

