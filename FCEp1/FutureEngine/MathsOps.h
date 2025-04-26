#pragma once
#include <glm/glm.hpp>
class MathsOps
{
public:

	static glm::vec2 TransformCoord(const glm::vec2& coord, float angleInDegrees=0.0f, float scale=1.0f);
	static float RndFloat(float min, float max);
	static float Deg2Rad(float rad);

};

