#include "GameAnimation.h"
#include "Texture2D.h"

GameAnimation::GameAnimation(std::string name)
{

	m_Name = name;

}


void GameAnimation::AddFrame(Texture2D* color, Texture2D* normal)
{
	if (normal == nullptr) {
		normal = new Texture2D("engine/maps/blanknormal.png");
	}
	AnimFrame* frame = new AnimFrame;
	frame->m_Color = color;
	frame->m_Normal = normal;
	m_Frames.push_back(frame);
}