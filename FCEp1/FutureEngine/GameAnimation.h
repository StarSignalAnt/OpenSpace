#pragma once
#include <vector>
#include <string>

class Texture2D;

struct AnimFrame {

	Texture2D* m_Color;
	Texture2D* m_Normal;

};

class GameAnimation
{
public:

	GameAnimation(std::string name);
	void AddFrame(Texture2D* color, Texture2D* normal);
	void SetSize(int width, int height) { m_FrameWidth = width; m_FrameHeight = height; }
	int GetFrameWidth() { return m_FrameWidth; }
	int GetFrameHeight() { return m_FrameHeight; }
	std::string GetName() { return m_Name; }
	void SetName(std::string name) { m_Name = name; }
	AnimFrame* GetCurrentFrame() { return m_Frames[m_CurrentFrame]; }


private:

	std::vector<AnimFrame*> m_Frames;
	std::string m_Name;
	int m_FrameWidth = 128;
	int m_FrameHeight = 128;
	float m_Time = 0;
	float m_Length = 1;
	int m_CurrentFrame = 0;

};

