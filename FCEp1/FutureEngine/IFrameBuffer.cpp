#include "IFrameBuffer.h"
#include "RenderTarget2D.h"
#include "UIHelp.h"
#include <iostream>
#include <glad/glad.h>
#include "Texture2D.h"

IFrameBuffer::IFrameBuffer(glm::vec2 position, glm::vec2 size) {

	m_RT = new RenderTarget2D(size.x,size.y);
	Set(position, size);
	tex = new Texture2D("test/test2.png");

}

void IFrameBuffer::PreRender() {


	UIHelp::RemoveScissor();
	if (((int)m_Size.x) != m_RT->GetWidth() || ((int)m_Size.y) != m_RT->GetHeight()) {
		m_RT = new RenderTarget2D(m_Size.x, m_Size.y);
		std::cout << "NEW RT" << std::endl;
		std::cout << "m_Size:" << m_Size.x << " " << m_Size.y << std::endl;
		std::cout << "RT:" << m_RT->GetWidth() << " " << m_RT->GetHeight() << std::endl;

	}

	//m_RT->Bind();


	//m_RT->Clear(glm::vec3(0,0,0));
	CallPreRender();
//	m_RT->Unbind();


	PreRenderChildren();


	//glClearColor(0.3, 0.3, 0.3, 1.0f);



}

void IFrameBuffer::Render() {


	auto pos = GetRenderPosition();
	auto size = m_Size;
//	size.y = -size.y;

	UIHelp::DrawImage(pos,size,m_RT->GetTexture(), m_Color);
	

}

void IFrameBuffer::Update(float delta) {


}

void IFrameBuffer::Bind() {

	m_RT->Bind();
	m_RT->Clear(glm::vec3(0, 0, 0));
}

void IFrameBuffer::Unbind() {
	m_RT->Unbind();
}