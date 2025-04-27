#pragma once
#include "UIHelp.h"
#include <vector>

class Texture2D;
class SmartDraw;

struct StarNode {

	glm::vec3 pos;
	glm::vec3 col;
	int next_c = 0;
	float col_mod = 0.0;

};

class Starfield
{
public:

	Starfield(int num);
	void Update(float dt);
	void Render();

private:

	std::vector<StarNode> m_Nodes;
	Texture2D* m_Star;
	glm::vec2 m_Dir;

};

