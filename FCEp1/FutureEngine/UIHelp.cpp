#include "UIHelp.h"
#include "SmartDraw.h"
#include "GameFont.h"
#include "ShaderModule.h"
#include "Texture2D.h"
#include "FutureApp.h"
#include "DrawInfo.h"

SmartDraw* m_Draw;
GameFont* m_UIFont;
SmartDraw* m_FontDraw;
SmartDraw* m_BlurDraw;
Texture2D* m_RectTex;
Texture2D* m_ColorBG;
glm::vec4 UIHelp::m_Scissor;


void UIHelp::InitHelp() {

	m_Draw = new SmartDraw;
	m_UIFont = new GameFont("engine/ui/uifont.ttf",16);
	m_FontDraw = new SmartDraw;
	m_BlurDraw = new SmartDraw;

	//m_BlurDraw->SetShaderModule(new ShaderModule("engine/shader/drawvs.glsl", "engine/shader/drawBlurFS.glsl"));
	//m_FontDraw->SetShaderModule(new ShaderModule("engine/shader/drawvs.glsl", "engine/shader/drawFont.glsl"));
	m_RectTex = new Texture2D("engine/ui/rect.png");
	m_ColorBG = new Texture2D("engine/ui/colorBG6.jpg");
	m_Scissor = glm::vec4(0, 0, FutureApp::m_Inst->GetWidth(), FutureApp::m_Inst->GetHeight()/2);

}

void UIHelp::DrawImageBlur(glm::vec2 position, glm::vec2 size, Texture2D* texture, glm::vec4 color,float blur)
{
	DrawImage(position, size, texture, color);
	return;
	glEnable(GL_BLEND);
	glClear(GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_BlurDraw->Begin();

//	m_BlurDraw->GetShaderModule()->Bind();
//	m_BlurDraw->GetShaderModule()->SetFloat("blurAmount", blur*500);
	m_BlurDraw->DrawDirect(position, size, color, texture);

	m_BlurDraw->End();

}

void UIHelp::DrawImage(glm::vec2 position, glm::vec2 size, Texture2D* texture, glm::vec4 color)
{
//	glEnable(GL_BLEND);
//	glClear(GL_DEPTH_BUFFER_BIT);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//m_Draw->Begin();
	auto inf = m_Draw->DrawDirect(position, size, color, texture);
	inf->SetView(m_Scissor);
	
//	m_Draw->End();

}


void UIHelp::DrawImage(glm::vec2 position, glm::vec2 size, Texture2D* texture, glm::vec4 color,float rot,float scale)
{
	//	glEnable(GL_BLEND);
	//	glClear(GL_DEPTH_BUFFER_BIT);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//m_Draw->Begin();
	auto inf = m_Draw->Draw(position, size, color, texture,rot,scale);
	inf->SetView(m_Scissor);

	//	m_Draw->End();

}

float UIHelp::StrWidth(std::string text,float scale) {

	return m_UIFont->StrWidth(text,scale);

}

float UIHelp::StrHeight(std::string text,float scale) {
	return m_UIFont->StrHeight(text,scale);
}

void UIHelp::DrawCharacter(glm::vec2 position, glm::vec2 size, Texture2D* texture, glm::vec4 color)
{
//	return;
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//m_FontDraw->Begin();
	auto inf = m_Draw->DrawDirect(position, size, color, texture);
	inf->SetView(m_Scissor);
	//m_FontDraw->End();


}


void UIHelp::DrawText(glm::vec2 position, std::string text, glm::vec4 color,float scale) {

	//glClear(GL_DEPTH_BUFFER_BIT);
//	m_FontDraw->Begin();
	m_UIFont->Render(position,text, color*2.5f,scale);


//	m_FontDraw->End();

}

void UIHelp::DrawRect(glm::vec2 pos, glm::vec2 size, glm::vec4 color)
{
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glDisable(GL_DEPTH_TEST);
	//m_Draw->Begin();
	auto inf = m_Draw->DrawDirect(pos, size, color,m_RectTex);
	inf->SetView(m_Scissor);

	//	/m_Draw->End();
	// 
	//glEnable(GL_DEPTH_TEST);
}

void UIHelp::DrawOutlineRect(glm::vec2 pos, glm::vec2 size, glm::vec4 color)
{

	DrawRect(pos, glm::vec2(size.x,1), color);
	DrawRect(pos, glm::vec2(1, size.y), color);
	DrawRect(pos + glm::vec2(0, size.y), glm::vec2(size.x, 1), color);
	DrawRect(pos + glm::vec2(size.x, 0), glm::vec2(1, size.y), color);
	return;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);
	m_Draw->Begin();
	m_Draw->DrawDirect(pos, size, color, m_RectTex);
	m_Draw->End();
	glEnable(GL_DEPTH_TEST);
}

void UIHelp::DrawImageWithBG(glm::vec2 pos, glm::vec2 size, glm::vec4 color)
{
	
	float yHigh = size.y / (float)FutureApp::m_Inst->GetHeight();
	float yPos = pos.y / (float)FutureApp::m_Inst->GetHeight();
	float xPos = pos.x / (float)FutureApp::m_Inst->GetWidth();
	float xHigh = xPos+size.x / (float)FutureApp::m_Inst->GetWidth();
	yHigh = yPos + yHigh;
//	glEnable(GL_BLEND);
//	glClear(GL_DEPTH_BUFFER_BIT);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	m_BlurDraw->Begin();

//	m_BlurDraw->GetShaderModule()->Bind();
//	m_BlurDraw->GetShaderModule()->SetFloat("blurAmount", 0.4 * 500);
	auto info = m_Draw->DrawDirect(pos, size, color,m_ColorBG);

	info->SetTexCoord(0, xPos, yPos);
	info->SetTexCoord(1, xHigh, yPos);
	info->SetTexCoord(2, xHigh, yHigh);
	info->SetTexCoord(3, xPos, yHigh);
	info->SetView(m_Scissor);

//	m_BlurDraw->End();

	

	return;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);
	m_Draw->Begin();
//	auto info = m_Draw->DrawDirect(pos, size, color, m_ColorBG);
	info->SetTexCoord(0,xPos, yPos);
	info->SetTexCoord(1, xHigh, yPos);
	info->SetTexCoord(2, xHigh, yHigh);
	info->SetTexCoord(3, xPos, yHigh);
	m_Draw->End();


	glEnable(GL_DEPTH_TEST);

}

// Add this function implementation to UIHelp.cpp:

void UIHelp::setScissor(int x, int y, int width, int height, int windowHeight) {
	// Convert from left-top (0,0) to OpenGL's left-bottom (0,0)
	// by flipping the Y coordinate
	m_Scissor = glm::vec4(x, y, width, height);
	return;
	int openglY = windowHeight - (y + height);

	// Set the scissor test
	glEnable(GL_SCISSOR_TEST);
	glScissor(x, openglY, width, height);

}

void UIHelp::RemoveScissor() {
	m_Scissor = glm::vec4(0, 0, FutureApp::m_Inst->GetWidth(), FutureApp::m_Inst->GetHeight());
	return;
	glDisable(GL_SCISSOR_TEST);

}

void UIHelp::ClearZ() {



}

void UIHelp::Begin() {

	m_Draw->Begin();

}

void UIHelp::End() {

	m_Draw->End();

}