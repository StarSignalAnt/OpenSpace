#include "GameSprite.h"
#include "GameAnimation.h"

GameSprite::GameSprite()
{

}

void GameSprite::AddAnimation(GameAnimation* anim)
{
	m_Animations.push_back(anim);
}

void GameSprite::SetAnimation(std::string name)
{
	for (auto anim : m_Animations) {
		if (anim->GetName() == name) {
			m_CurrentAnimation = anim;
			return;
		}
	}
}

AnimFrame* GameSprite::GetCurrentFrame()
{
	if (m_CurrentAnimation == nullptr) return nullptr;
	return m_CurrentAnimation->GetCurrentFrame();
}