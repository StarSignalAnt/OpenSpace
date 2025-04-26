#include "SmartDraw.h"
#include "Texture2D.h"
#include "MathsOps.h"
#include "DrawList.h"
#include "DrawInfo.h"
#include "ShaderModule.h"
#include <glm/glm.hpp>
#include "FutureApp.h"
#include "GPSmartDraw.h"
#include <glm/gtc/matrix_transform.hpp>
#include "MeshBuffer.h"


MeshBuffer* SmartDraw::m_PBuf = nullptr;

SmartDraw::SmartDraw()
{


//	m_SM = new ShaderModule("engine/shader/drawVS.glsl", "engine/shader/drawFS.glsl");
//	VAO = 0;
//	VBO = 0;
//	EBO = 0;
	m_Pipeline = new GPSmartDraw;

	if (m_PBuf == nullptr) {

		m_PBuf = new MeshBuffer(4024, 4024);

	}

}

void SmartDraw::Begin()
{

	for (auto list : m_DrawLists)
	{
		list->Free();
		delete list;
	}
	m_DrawLists.clear();
	
	m_CurrentZ = 0.91f;

//	m_SMSimple->Bind();
//	m_SMSimple->SetUniformMat4("u_Projection", m_ProjectionMatrix);

}

DrawInfo* SmartDraw::Draw(glm::vec2 position, glm::vec2 size, glm::vec4 color, Texture2D* texture, float rotation, float scale)
{

	auto list = GetList(texture);

	DrawInfo* info = new DrawInfo;

	float tx0, tx1, tx2, tx3;
	float ty0, ty1, ty2, ty3;

	tx0 = -size.x / 2;
	tx1 = size.x / 2;
	tx2 = size.x / 2;
	tx3 = -size.x / 2;

	ty0 = -size.y / 2;

	ty1 = -size.y / 2;
	ty2 = size.y / 2;
	ty3 = size.y / 2;

	auto v1 = MathsOps::TransformCoord(glm::vec2(tx0, ty0), rotation, scale);
	auto v2 = MathsOps::TransformCoord(glm::vec2(tx1, ty1), rotation, scale);
	auto v3 = MathsOps::TransformCoord(glm::vec2(tx2, ty2), rotation, scale);
	auto v4 = MathsOps::TransformCoord(glm::vec2(tx3, ty3), rotation, scale);

	info->SetCoord(0, position.x + v1.x, position.y + v1.y);
	info->SetCoord(1, position.x + v2.x, position.y + v2.y);
	info->SetCoord(2, position.x + v3.x, position.y + v3.y);
	info->SetCoord(3, position.x + v4.x, position.y + v4.y);




//	info->SetCoord(0, x0, y0);
//	info->SetCoord(1, x1, y1);
//	info->SetCoord(2, x2, y2);
//	info->SetCoord(3, x3, y3);


	info->SetTexCoord(0, 0, 0);
	info->SetTexCoord(1, 1, 0);
	info->SetTexCoord(2, 1, 1);
	info->SetTexCoord(3, 0, 1);
	
	info->SetColor(color);

	info->SetTexture(texture);

	info->SetZ(m_CurrentZ);

	m_CurrentZ -= 0.00001f;

	list->Add(info);

	return info;


}


DrawInfo* SmartDraw::DrawDirect(glm::vec2 position, glm::vec2 size, glm::vec4 color, Texture2D* texture)
{

	auto list = GetList(texture);

	DrawInfo* info = new DrawInfo;



	

	info->SetCoord(0, position.x , position.y);
	info->SetCoord(1, position.x + size.x, position.y);
	info->SetCoord(2, position.x + size.x, position.y + size.y);
	info->SetCoord(3, position.x , position.y + size.y);




	//	info->SetCoord(0, x0, y0);
	//	info->SetCoord(1, x1, y1);
	//	info->SetCoord(2, x2, y2);
	//	info->SetCoord(3, x3, y3);


	info->SetTexCoord(0, 0, 0);
	info->SetTexCoord(1, 1, 0);
	info->SetTexCoord(2, 1, 1);
	info->SetTexCoord(3, 0, 1);

	info->SetColor(color);

	info->SetTexture(texture);

	info->SetZ(m_CurrentZ);

	m_CurrentZ -= 0.00001f;

	list->Add(info);

	return info;


}

void SmartDraw::End()
{


//	m_Pipeline->Bind();

//	m_Pipeline->Render();
//	return;

	float4x4 m_ProjectionMatrix = float4x4::Ortho(static_cast<float>(FutureApp::m_Inst->GetWidth()),
		static_cast<float>(FutureApp::m_Inst->GetHeight()),
		0, 1.0f, false);
	
	m_ProjectionMatrix = float4x4::OrthoOffCenter(0, (float)FutureApp::m_Inst->GetWidth(), (float)FutureApp::m_Inst->GetHeight(),0,-1.0f,1.01f,true);


	//m_ProjectionMatrix = glm::ortho(0.0f, (float)FutureApp::m_Inst->GetWidth(), (float)FutureApp::m_Inst->GetHeight(), 0.0f, -1.0f, 1.0f);
	if (pdata == nullptr) {

		//pdata = (float*)malloc(100000 * 9 * 4)


	}
//	if (VAO!=0)
	{
//		glBindVertexArray(VAO);
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	}
	// Iterate over the draw lists
	for (auto list : m_DrawLists) {
//		m_SM->Bind();


//		m_SM->SetMat4("uProjection", m_ProjectionMatrix);
//
		auto tex = list->GetList()[0]->GetTexture();
		tex->Bind(0);
		auto norm = list->GetList()[0]->GetNormalTexture();
		if (norm != nullptr) {
			norm->Bind(1);
		//	m_SM->SetInt("feNormalMap", 1);
		}

		//m_SM->SetInt("uTexture", 0);
		//m_SM->SetFloat("TopY", FutureApp::m_Inst->GetHeight());
	

		// Get packed vertex data
		MeshBuffer* data = GetData(list);

		m_Pipeline->SetWorld(m_ProjectionMatrix);

		m_Pipeline->SetAux(norm);
		m_Pipeline->SetOther(list->GetList()[0]->GetAux());
		m_Pipeline->Bind(data,list->GetTexture());
		m_Pipeline->Render(data);






		// Create VAO once if not created
		//if (VAO == 0) {
		//	glGenVertexArrays(1, &VAO);
		//	glBindVertexArray(VAO);
		//}


		// Create VBO if not created, and allocate buffer space with correct size
	//	if (VBO == 0) {
		//	glGenBuffers(1, &VBO);
		//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//	glBufferData(GL_ARRAY_BUFFER, MAX_VERTEX_SIZE*2, nullptr, GL_DYNAMIC_DRAW);  // Allocate the buffer with the correct size
		//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//}

		// Upload vertex data to VBO
		//glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, data);




		// Define vertex attributes
		GLsizei stride = 20 * sizeof(float);

		// Position (vec3)


		/*
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(0);

		// TexCoord (vec2)
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Color (vec4)
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (void*)(12 * sizeof(float)));
		glEnableVertexAttribArray(4);

		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, stride, (void*)(16 * sizeof(float)));
		glEnableVertexAttribArray(5);
		// Draw the elements
		//glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

		glDrawArrays(GL_TRIANGLES, 0, list->GetList().size() * 6);
		*/


		// Free the vertex data after use
	//	free(data);

		// Unbind texture and reset the shader program
		tex->Unbind(0);
		if (norm != nullptr) {
			norm->Unbind(1);
		}
	}

	// Unbind the VAO and shader
	//glBindVertexArray(0);
//	m_SM->Unbind();
}
DrawList* SmartDraw::GetList(Texture2D* texture)
{
	for (auto list : m_DrawLists)
	{
		if (list->GetTexture() == texture)
		{
			return list;
		}
	}
	auto list = new DrawList();
	list->SetTexture(texture);
	m_DrawLists.push_back(list);
	return list;
}

inline void SetVertexData(float* data, int& index, const glm::vec2& pos, float z, const glm::vec2& texCoord, const glm::vec4& color,const glm::vec3& real,const glm::vec4& extra,const glm::vec4& view) {
	data[index++] = pos.x;
	data[index++] = pos.y;
	data[index++] = z;
	data[index++] = texCoord.x;
	data[index++] = texCoord.y;
	data[index++] = color.r;
	data[index++] = color.g;
	data[index++] = color.b;
	data[index++] = color.a;
	data[index++] = real.x;
	data[index++] = real.y;
	data[index++] = real.z;
	data[index++] = extra.x;
	data[index++] = extra.y;
	data[index++] = extra.z;
	data[index++] = extra.w;
	data[index++] = view.x;
	data[index++] = view.y;
	data[index++] = view.z;
	data[index++] = view.w;


}

MeshBuffer* SmartDraw::GetData(DrawList* list) {
	// Each info will now require 6 vertices instead of 4
	

	auto buf = m_PBuf;

	buf->Clear();

	int index = 0;

	for (auto info : list->GetList()) {
		// Retrieve the coordinates of the 4 corners of the quad
		auto pos0 = info->GetCoord(0);
		auto pos1 = info->GetCoord(1);
		auto pos2 = info->GetCoord(2);
		auto pos3 = info->GetCoord(3);

		// Common Z coordinate and texture coordinates, color values
		float z = info->GetZ();
		auto texCoord0 = info->GetTexCoord(0);
		auto texCoord1 = info->GetTexCoord(1);
		auto texCoord2 = info->GetTexCoord(2);
		auto texCoord3 = info->GetTexCoord(3);
		auto color = info->GetColor();

		auto real0 = info->GetRealCoord(0);
		auto real1 = info->GetRealCoord(1);
		auto real2 = info->GetRealCoord(2);
		auto real3 = info->GetRealCoord(3);

		auto extra = info->GetExtra();

		auto view = info->GetView();

		Vertex v0, v1, v2, v3;

		v0.pos = float3(pos0.x, pos0.y, z);
		v1.pos = float3(pos1.x, pos1.y, z);
		v2.pos = float3(pos2.x, pos2.y, z);
		v3.pos = float3(pos3.x, pos3.y, z);
		v0.col = float4(color.x,color.y,color.z,color.w);
		v1.col = v0.col;
		v2.col = v1.col;
		v3.col = v1.col;
		v0.uv = float2(texCoord0.x, texCoord0.y);
		v1.uv = float2(texCoord1.x, texCoord1.y);
		v2.uv = float2(texCoord2.x, texCoord2.y);
		v3.uv = float2(texCoord3.x, texCoord3.y);
		v0.view = float4(view.x, view.y, view.z, view.w);
		v1.view = v0.view;
		v2.view = v0.view;
		v3.view = v0.view;
		v0.realPos = float3(real0.x, real0.y, real0.z);
		v1.realPos = float3(real1.x, real1.y, real1.z);
		v2.realPos = float3(real2.x, real2.y, real2.z);
		v3.realPos = float3(real3.x, real3.y, real3.z);
		v0.extra = float4(extra.x, extra.y, extra.z, extra.w);
		v1.extra = v0.extra;
		v2.extra = v0.extra;
		v3.extra = v0.extra;



		buf->AddVertex(v0);
		buf->AddVertex(v1);
		buf->AddVertex(v2);
		buf->AddVertex(v3);
	
		Tri t1, t2;

		t1.indices = int3(index, index + 1, index + 2);
		t2.indices = int3(index, index + 2, index + 3);

		buf->AddTriangle(t1);
		buf->AddTriangle(t2);

		Tri t3, t4;
		t3.indices = int3(index, index + 2, index + 1);
		t4.indices = int3(index, index + 3, index + 2);

	//	buf->AddTriangle(t3);
	//	buf->AddTriangle(t4);

		index += 4;

		// First triangle (using vertices 0, 1, 2)
//		SetVertexData(data, index, pos0, z, texCoord0, color, real0, extra,view);
//		SetVertexData(data, index, pos1, z, texCoord1, color, real1, extra,view);
	//	SetVertexData(data, index, pos2, z, texCoord2, color, real2, extra,view);

		// Second triangle (using vertices 2, 3, 0)
//		SetVertexData(data, index, pos2, z, texCoord2, color, real2, extra,view);
//		SetVertexData(data, index, pos3, z, texCoord3, color, real3, extra,view);
	//	SetVertexData(data, index, pos0, z, texCoord0, color, real0, extra,view);

	}

	buf->Finalize();

	return buf;
}

void SmartDraw::SetShaderModule(ShaderModule* module) {

//	m_SM = module;

}