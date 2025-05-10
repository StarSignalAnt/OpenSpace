#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

class SmartDraw;
class RenderTarget2D;
class GraphicsPipeline;
class Texture2D;

class PostProcess
{
public:

	void CreateTarget(int w, int h);
	void CreateTargets(int num, int w, int h);
	void InitProcess();
	void BindTarget(int index, glm::vec3 col = glm::vec3(0, 0, 0));
	void ReleaseTarget(int index);
	RenderTarget2D* GetTarget(int index);
	void Draw(RenderTarget2D* target,int w=-1,int h=-1);
	void Draw(RenderTarget2D* target,GraphicsPipeline* gp, int w = -1, int h = -1);
	void Draw2(RenderTarget2D* target,Texture2D* tex, GraphicsPipeline* gp, int w = -1, int h = -1);
	virtual RenderTarget2D* Process(RenderTarget2D* input) = 0;


private:

	std::vector<RenderTarget2D*> m_Targets;
	SmartDraw* m_Draw;

};

