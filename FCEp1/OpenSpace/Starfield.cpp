#include "Starfield.h"
#include "MathsOps.h"
#include "FutureApp.h"
#include "Texture2D.h"
#include "UIHelp.h"

Starfield::Starfield(int num) {

	int sw = FutureApp::m_Inst->GetWidth();
	int sh = FutureApp::m_Inst->GetHeight();
	m_Dir = glm::vec2(-10, 0);

	for (int i = 0;i < num;i++) {

		StarNode node;

		float x = MathsOps::RndFloat(-100, sw + 100);
		float y = MathsOps::RndFloat(-100, sh + 100);
		float z = MathsOps::RndFloat(0.01, 1);

		node.pos = glm::vec3(x, y, z);
		node.col = glm::vec3(MathsOps::RndFloat(0, 1), MathsOps::RndFloat(0, 1), MathsOps::RndFloat(0, 1));

		node.col_mod = MathsOps::RndFloat(0, 1);
		if (node.col_mod > 0.5) {
			node.col_mod = 1;
			node.next_c = clock() + (MathsOps::RndFloat(50,300));
		}
		else {
			node.col_mod = 0.5;
			node.next_c = clock() + (MathsOps::RndFloat(1000, 3000));
		}

		m_Nodes.push_back(node);

	}

	m_Star = new Texture2D("content/sprites/stars/star1.png");

}

void Starfield::Update(float dt) {

	int sw = FutureApp::m_Inst->GetWidth();
	int sh = FutureApp::m_Inst->GetHeight();
	for (auto& node : m_Nodes) {

		
		node.pos = node.pos + glm::vec3(m_Dir.x, m_Dir.y, 0) * node.pos.z * (dt*10);

		if (node.pos.x < -100) {
			node.pos.x = sw + 100;
			node.pos.y = MathsOps::RndFloat(-100, sh + 100);
		}

		if (clock() > node.next_c) {

			if (MathsOps::RndFloat(1, 1000) > 780) {

				node.col_mod = MathsOps::RndFloat(0, 1);
				if (node.col_mod > 0.5) {
					node.col_mod = 1;
					node.next_c = clock() + (MathsOps::RndFloat(50, 300));
				}
				else {
					node.col_mod = 0.5;
					node.next_c = clock() + (MathsOps::RndFloat(1000, 3000));
				}

			}

		}

	}

}

void Starfield::Render() {

	UIHelp::Begin();

	for (auto node : m_Nodes) {

		glm::vec2 pos;
		glm::vec2 size = glm::vec2(16*node.pos.z, 16*node.pos.z);

		pos.x = node.pos.x;
		pos.y = node.pos.y;

		float cv = 1.0 * node.pos.z;
	

		glm::vec4 col = glm::vec4(cv,cv,cv, 1.0);
		col.x = 0;
		col.y = col.y * node.col.z * 2;
		col.z = col.z * node.col.z * 2;
	
		UIHelp::DrawImage(pos, size, m_Star,col*node.col_mod);

	}

	UIHelp::End();

}