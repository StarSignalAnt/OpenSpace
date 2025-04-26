#include "GameMap.h"
#include "GameTile.h"
#include "GameCam.h"
#include "GameLight.h"
#include "SmartDraw.h"
#include "FutureApp.h"
#include "MathsOps.h"
#include "ShaderModule.h"
#include "DrawInfo.h"
#include "RenderTarget2D.h"
#include "Texture2D.h"
#include "GameSprite.h"
#include "GameAnimation.h"
#include "UIHelp.h"
#include <cmath>
#include <array>
#include "GPMap2D.h"
#include <cmath>
#include "BinaryFile.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
void GameMap::InitMap() {

	m_ShadowRenderer = new SmartDraw;
//	m_DrawLit = new ShaderModule("engine/shader/drawLitVS.glsl", "engine/shader/drawLitFS.glsl");
//	m_ShadowWrite = new ShaderModule("engine/shader/drawvs.glsl", "engine/shader/shadowwriterfs.glsl");		
	
	m_TileRenderer->SetShaderModule(m_DrawLit);
	m_ShadowRenderer->SetShaderModule(m_ShadowWrite);
	m_ShadowRT = new RenderTarget2D(m_ShadowMapSize, m_ShadowMapSize);
	m_GridTex = new Texture2D("apps/mapeditor/grid.png");
    m_SelectTex = new Texture2D("engine/maps/white.png");
    m_LightIcon = new Texture2D("apps/mapeditor/light.png");

    auto gpm = new GPMap2D;
    m_M2D = gpm;
	m_TileRenderer->SetPipeline(gpm);

}

void GameMap::RenderShadowMap() {

	m_ShadowRT->Bind();
	m_ShadowRT->Clear(glm::vec3(1, 1, 1));

	float totalWidth = m_TileWidth * m_Width;
	float totalHeight = m_TileHeight * m_Height;

	float xScale = m_ShadowMapSize / totalWidth;
	float yScale = m_ShadowMapSize / totalHeight;

    UIHelp::RemoveScissor();


	m_ShadowRenderer->Begin();

	for (int z = 0; z < m_Depth; z++) {
		for (int y = 0; y < m_Height; y++) {
			for (int x = 0; x < m_Width; x++) {

				float drawX = (x * m_TileWidth) * xScale;
				float drawY = (y * m_TileHeight) * yScale;

				auto tile = GetTile(x, y, z);

				if (tile != nullptr) {

					if (tile->GetCastShadows()) {

						auto inf = m_ShadowRenderer->DrawDirect(glm::vec2(drawX, drawY), glm::vec2(m_TileWidth * xScale, m_TileHeight * yScale), glm::vec4(0,0, 0, 1), tile->GetFrame(0)[0]);
                        inf->SetView(UIHelp::m_Scissor);

					}

				}


			}
		}
	}

	for (auto spr : m_Sprites) {


		float spriteWidth = spr->GetCurrentAnimation()->GetFrameWidth() * xScale;
		float spriteHeight = spr->GetCurrentAnimation()->GetFrameHeight() * yScale;
		float drawX = spr->GetPosition().x * xScale;
		float drawY = spr->GetPosition().y * yScale;



		//m_ShadowRenderer->DrawDirect(glm::vec2(drawX, drawY), glm::vec2(spriteWidth,spriteHeight), glm::vec4(1, 1, 1, 1), spr->GetCurrentFrame()->m_Color);
		m_ShadowRenderer->Draw(glm::vec2(drawX, drawY), glm::vec2(spriteWidth, spriteHeight),glm::vec4(1,1,1,1),spr->GetCurrentFrame()->m_Color, -spr->GetRotation().y,1.0f);


	}

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

	m_ShadowRenderer->End();

	m_ShadowRT->Unbind();
//	glClearColor(0, 0, 0, 1);

//	m_TestRender->Begin();
//	m_TestRender->DrawDirect(glm::vec2(0, FutureApp::m_Inst->GetHeight()), glm::vec2(FutureApp::m_Inst->GetWidth(),-FutureApp::m_Inst->GetHeight()), glm::vec4(1, 1, 1, 1), m_ShadowRT->GetTexture());
//	m_TestRender->End();


}

void GameMap::RenderMap(GameCam* camera)
{
	UIHelp::RemoveScissor();

    if (m_Lights.size() == 0)
    {
        m_DW = FutureApp::m_Inst->GetWidth();
        m_DH = FutureApp::m_Inst->GetHeight();
        float midX = FutureApp::m_Inst->GetWidth() / 2.0f;
        float midY = FutureApp::m_Inst->GetHeight() / 2.0f;

        m_TestRender->Begin();

     ;

        for (int z = 0; z < m_Depth; z++) {
            for (int y = 0; y < m_Height; y++) {

                for (int x = 0; x < m_Width; x++)
                {

                    float drawX = x * m_TileWidth + m_TileWidth / 2;
                    float drawY = y * m_TileHeight + m_TileHeight / 2;

                    drawX -= camera->GetPosition().x;

                    drawY -= camera->GetPosition().y;


                    float pX = drawX - midX;
                    float pY = drawY - midY;

                    float rot = camera->GetRotation().y;

                    auto renderPos = MathsOps::TransformCoord(glm::vec2(pX, pY), rot, camera->GetPosition().z);

                    renderPos = glm::vec2(midX, midY) +
                        renderPos;


                    auto tile = GetTile(x, y, z);


                    if (tile != nullptr) {



                        auto info = m_TestRender->Draw(renderPos, glm::vec2(m_TileWidth, m_TileHeight), glm::vec4(1, 1,1, 1),tile->GetFrame(0)[0], rot, camera->GetPosition().z);
                        info->SetView(UIHelp::m_Scissor);
                        //extra 0 = cast shadows, extra 2 = receive shadows, extra 3 = recieve light


                    }

                }

            }
        }
        RenderGrid(camera);
        m_TestRender->End();


        return;

    }
    UIHelp::RemoveScissor();

//	RenderShadowMap();


	//
	float midX = FutureApp::m_Inst->GetWidth() / 2.0f;
	float midY = FutureApp::m_Inst->GetHeight() / 2.0f;

	m_TileRenderer->Begin();
	for (int z = 0; z < m_Depth; z++) {
		for (int y = 0; y < m_Height; y++) {

			for (int x = 0; x < m_Width; x++)
			{

				float drawX = x * m_TileWidth + m_TileWidth / 2;
				float drawY = y * m_TileHeight + m_TileHeight / 2;

				drawX -= camera->GetPosition().x;

				drawY -= camera->GetPosition().y;


				float pX = drawX - midX;
				float pY = drawY - midY;

				float rot = camera->GetRotation().y;
				
				auto renderPos = MathsOps::TransformCoord(glm::vec2(pX, pY),rot, camera->GetPosition().z);

				renderPos = glm::vec2(midX, midY) + 
					renderPos;	


				auto tile = GetTile(x, y, z);

				if (tile != nullptr) {
					auto info = m_TileRenderer->Draw(renderPos, glm::vec2(m_TileWidth, m_TileHeight), glm::vec4(1, 1, 1, 1), tile->GetFrame(0)[0],rot,camera->GetPosition().z);
					//RenderTile(drawX, drawY, tile);

                    info->SetAux(m_ShadowRT->GetTexture());
					int sX = x * m_TileWidth;
					int sY = y * m_TileHeight;

					info->SetNormalTexture(tile->GetFrame(0)[1]);
					info->SetRealCoord(0, sX,sY);
					info->SetRealCoord(1, sX + m_TileWidth,sY);
					info->SetRealCoord(2, sX+ m_TileWidth, sY + m_TileHeight);
					info->SetRealCoord(3, sX, sY + m_TileHeight);
                    info->SetView(UIHelp::m_Scissor);
					glm::vec4 ext(0, 0, 0, 0);

					if (tile->GetCastShadows()) {

						ext.x = 1.0;

					}

					if (tile->GetReceivesShadows()) {
						ext.y = 1.0;
					}
					else {
						int a = 5;
					}

					if (tile->GetReceivesLight()) {
						ext.z = 1.0;
					}

					info->SetExtra(ext);

					//extra 0 = cast shadows, extra 2 = receive shadows, extra 3 = recieve light


				}

			}

		}
	}

	for (auto spr : m_Sprites) {

		float drawX = spr->GetPosition().x;
		float drawY = spr->GetPosition().y;

		drawX -= camera->GetPosition().x;

		drawY -= camera->GetPosition().y;


		float pX = drawX - midX;
		float pY = drawY - midY;

		float rot = camera->GetRotation().y;

		auto renderPos = MathsOps::TransformCoord(glm::vec2(pX, pY), rot, camera->GetPosition().z);

		renderPos = glm::vec2(midX, midY) +
			renderPos;

		auto frame = spr->GetCurrentFrame();

		auto anim = spr->GetCurrentAnimation();

		auto info = m_TileRenderer->Draw(renderPos, glm::vec2(anim->GetFrameWidth(),anim->GetFrameHeight()), glm::vec4(1, 1, 1, 1), frame->m_Color, rot-spr->GetRotation().y, camera->GetPosition().z);
		info->SetNormalTexture(frame->m_Normal);
		
		glm::vec4 ext(0, 0, 0, 0);

		if (spr->GetCastShadows()) {

			ext.x = 1.0;

		}

		if (spr->GetReceivesShadows()) {
			ext.y = 1.0;
		}
		else {
			int a = 5;
		}

		if (spr->GetReceivesLight()) {
			ext.z = 1.0;
		}

		float sX = spr->GetPosition().x - anim->GetFrameWidth() / 2;
		float sY = spr->GetPosition().y - anim->GetFrameHeight() / 2;



		info->SetRealCoord(0, sX, sY);
		info->SetRealCoord(1, sX + anim->GetFrameWidth(), sY);
		info->SetRealCoord(2, sX + anim->GetFrameWidth(), sY + anim->GetFrameHeight());
		info->SetRealCoord(3, sX, sY + anim->GetFrameHeight());

		info->SetExtra(ext);
		
	

	}

//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (auto light : m_Lights) {

		//m_DrawLit->Bind();


		glm::vec3 lightPos = light->GetPosition();

		lightPos -= glm::vec3(camera->GetPosition().x, camera->GetPosition().y, 0);

		//

		float pX = lightPos.x - midX;
		float pY = lightPos.y - midY;

		float rot = camera->GetRotation().y;

		auto renderPos = MathsOps::TransformCoord(glm::vec2(pX, pY), rot, camera->GetPosition().z);

		lightPos = glm::vec3(midX, midY, lightPos.z) +
			glm::vec3(renderPos.x,renderPos.y,0);




		//

        /*
		m_DrawLit->SetInt("feShadowMap", 2);
		m_DrawLit->SetInt("feNormalMap", 1);
		m_DrawLit->SetVec2("feScreenSize", glm::vec2(FutureApp::m_Inst->GetWidth(), FutureApp::m_Inst->GetHeight()));
		m_DrawLit->SetVec3("feLightPos", lightPos);
		m_DrawLit->SetFloat("feLightRange", light->GetRange()*camera->GetPosition().z);
		m_DrawLit->SetFloat("feCamRot", camera->GetRotation().y);
		m_DrawLit->SetFloat("feCamZoom", camera->GetPosition().z);
		m_DrawLit->SetVec2("feLightActual", glm::vec2(light->GetPosition().x, light->GetPosition().y));
        m_DrawLit->SetVec3("feLightDiffuse", light->GetDiffuse());
		m_DrawLit->SetVec2("feMapSize", glm::vec2(m_Width * m_TileWidth, m_Height * m_TileHeight));
		m_ShadowRT->GetTexture()->Bind(2);
		*/

        m_M2D->screenSize = float4(FutureApp::m_Inst->GetWidth(), FutureApp::m_Inst->GetHeight(), 0, 0);
		m_M2D->lightPos = float4(lightPos.x, lightPos.y, lightPos.z, 0);
        m_M2D->lightRange = float4(light->GetRange() * camera->GetPosition().z);
		m_M2D->camRot = float4(camera->GetRotation().y, 0, 0, 0);
		m_M2D->camZoom = float4(camera->GetPosition().z, 0, 0, 0);
        m_M2D->lightActual = float4(light->GetPosition().x, light->GetPosition().y, 0, 0);
		m_M2D->lightDiffuse = float4(light->GetDiffuse().x, light->GetDiffuse().y, light->GetDiffuse().z, 0);
		m_M2D->mapSize = float4(m_Width * m_TileWidth, m_Height * m_TileHeight, 0, 0);
        //m_M2D->lightColor = float4(light->GetColor().x, light->GetColor().y, light->GetColor().z, 0);



        m_TileRenderer->End();
		//m_ShadowRT->GetTexture()->Unbind(2);


	}
   // UIHelp::ClearZ();
    
    m_TestRender->Begin();
    RenderGrid(camera);
    m_TestRender->End();



}

void GameMap::AddLight(GameLight* light) {

	m_Lights.push_back(light);

}

void GameMap::FIllBlock(GameTile* tile, int x, int y, int w, int h, int z)
{
	for (int j = y; j < y + h; j++) {
		for (int i = x; i < x + w; i++) {
			SetTile(i, j, z, tile);
		}
	}
}

void GameMap::RenderGrid(GameCam* camera) {

	m_DW = FutureApp::m_Inst->GetWidth();
	m_DH = FutureApp::m_Inst->GetHeight();
	float midX = FutureApp::m_Inst->GetWidth() / 2.0f;
	float midY = FutureApp::m_Inst->GetHeight() / 2.0f;

	//m_TestRender->Begin();
    FutureApp::m_Inst->ClearZ();

	for (int y = 0; y < m_Height; y++) {

		for (int x = 0; x < m_Width; x++)
		{

			float drawX = x * m_TileWidth + m_TileWidth / 2;
			float drawY = y * m_TileHeight + m_TileHeight / 2;

			drawX -= camera->GetPosition().x;

			drawY -= camera->GetPosition().y;


			float pX = drawX - midX;
			float pY = drawY - midY;

			float rot = camera->GetRotation().y;

			auto renderPos = MathsOps::TransformCoord(glm::vec2(pX, pY), rot, camera->GetPosition().z);

			renderPos = glm::vec2(midX, midY) +
				renderPos;


			//auto tile = GetTile(x, y, z);


		//	if (tile != nullptr) {

		
			auto info = m_TestRender->Draw(renderPos, glm::vec2(m_TileWidth, m_TileHeight), glm::vec4(0, 1, 0, 1), m_GridTex, rot, camera->GetPosition().z);
            info->SetView(UIHelp::m_Scissor);
			//extra 0 = cast shadows, extra 2 = receive shadows, extra 3 = recieve light


		//}

		}

	}

	//m_TestRender->End();



    //return;


    //m_TestRender->Begin();

	for (int y = 0; y < m_Height; y++) {

		for (int x = 0; x < m_Width; x++)
		{

			float drawX = x * m_TileWidth + m_TileWidth / 2;
			float drawY = y * m_TileHeight + m_TileHeight / 2;

			drawX -= camera->GetPosition().x;

			drawY -= camera->GetPosition().y;


			float pX = drawX - midX;
			float pY = drawY - midY;

			float rot = camera->GetRotation().y;

			auto renderPos = MathsOps::TransformCoord(glm::vec2(pX, pY), rot, camera->GetPosition().z);

			renderPos = glm::vec2(midX, midY) +
				renderPos;


			//auto tile = GetTile(x, y, z);


		//	if (tile != nullptr) {

			if (GetHighlight(x, y)) {
				auto info = m_TestRender->Draw(renderPos, glm::vec2(m_TileWidth, m_TileHeight), glm::vec4(1, 1, 1, 0.75), m_SelectTex, rot, camera->GetPosition().z);
				//auto info2 = m_TestRender->Draw(renderPos, glm::vec2(m_TileWidth-8, m_TileHeight-8), glm::vec4(1, 1, 1, 1), m_GridTex, rot, camera->GetPosition().z);
                info->SetView(UIHelp::m_Scissor);
                //info2->SetView(UIHelp::m_Scissor);
			}
			
			//extra 0 = cast shadows, extra 2 = receive shadows, extra 3 = recieve light


		//}

		}

	}

	//m_TestRender->End();

}

bool IsPointInTriangle(const glm::vec2& point, const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3) {
	// Compute barycentric coordinates
	float denominator = ((v2.y - v3.y) * (v1.x - v3.x) + (v3.x - v2.x) * (v1.y - v3.y));

	// Check for degenerate triangle
	if (denominator == 0) {
		return false;
	}

	float a = ((v2.y - v3.y) * (point.x - v3.x) + (v3.x - v2.x) * (point.y - v3.y)) / denominator;
	float b = ((v3.y - v1.y) * (point.x - v3.x) + (v1.x - v3.x) * (point.y - v3.y)) / denominator;
	float c = 1.0f - a - b;

	// Check if point is inside triangle
	return (a >= 0.0f && a <= 1.0f &&
		b >= 0.0f && b <= 1.0f &&
		c >= 0.0f && c <= 1.0f);
}


SelectionList GameMap::SelectPoint(GameCam* camera, int mx, int my)
{

	float midX = m_DW / 2.0f;
	float midY = m_DH / 2.0f;



	for (int y = 0; y < m_Height; y++) {

		for (int x = 0; x < m_Width; x++)
		{

			float drawX = x * m_TileWidth + m_TileWidth / 2;
			float drawY = y * m_TileHeight + m_TileHeight / 2;

			drawX -= camera->GetPosition().x;

			drawY -= camera->GetPosition().y;


			float pX = drawX - midX;
			float pY = drawY - midY;

			float rot = camera->GetRotation().y;

			auto renderPos = MathsOps::TransformCoord(glm::vec2(pX, pY), rot, camera->GetPosition().z);

			renderPos = glm::vec2(midX, midY) +
				renderPos;


			//auto tile = GetTile(x, y, z);


			float tx0, tx1, tx2, tx3;
			float ty0, ty1, ty2, ty3;

			tx0 = -m_TileWidth / 2;
			tx1 = m_TileWidth / 2;
			tx2 = m_TileWidth / 2;
			tx3 = -m_TileWidth / 2;

			ty0 = -m_TileHeight / 2;

			ty1 = -m_TileHeight / 2;
			ty2 = m_TileHeight / 2;
			ty3 = m_TileHeight / 2;

			float scale = camera->GetPosition().z;

			auto v1 = MathsOps::TransformCoord(glm::vec2(tx0, ty0), rot, scale);
			auto v2 = MathsOps::TransformCoord(glm::vec2(tx1, ty1), rot, scale);
			auto v3 = MathsOps::TransformCoord(glm::vec2(tx2, ty2), rot, scale);
			auto v4 = MathsOps::TransformCoord(glm::vec2(tx3, ty3), rot, scale);


			v1 = glm::vec2(renderPos.x + v1.x, renderPos.y + v1.y);
			v2 = glm::vec2(renderPos.x + v2.x, renderPos.y + v2.y);
			v3 = glm::vec2(renderPos.x + v3.x, renderPos.y + v3.y);
			v4 = glm::vec2(renderPos.x + v4.x, renderPos.y + v4.y);

			if (IsPointInTriangle(glm::vec2(mx, my), v1, v2, v3) || IsPointInTriangle(glm::vec2(mx, my), v3, v4, v1)){

				auto list = Select(x, y, 0);
				return list;

			}

			//info->SetCoord(0, render.x + v1.x, position.y + v1.y);
			//info->SetCoord(1, position.x + v2.x, position.y + v2.y);
			//info->SetCoord(2, position.x + v3.x, position.y + v3.y);
			//info->SetCoord(3, position.x + v4.x, position.y + v4.y);


		//	if (tile != nullptr) {

			//if (GetHighlight(x, y)) {

			//	auto info = m_TestRender->Draw(renderPos, glm::vec2(m_TileWidth, m_TileHeight), glm::vec4(0, 1, 1, 1), m_GridTex, rot, camera->GetPosition().z);
		//	}
	//		else {
		//		auto info = m_TestRender->Draw(renderPos, glm::vec2(m_TileWidth, m_TileHeight), glm::vec4(0, 1, 0, 1), m_GridTex, rot, camera->GetPosition().z);
			//}
			//extra 0 = cast shadows, extra 2 = receive shadows, extra 3 = recieve light


		//}

		}

	}

	return SelectionList();

}


void GameMap::ClearHighlights() {

	for (int y = 0; y < m_Height; y++) {
		for (int x = 0; x < m_Width; x++) {

			SetHighlight(x, y, false);

		}
	}

}

bool GameMap::LineSegmentsIntersect(const glm::vec2& p1, const glm::vec2& p2,
    const glm::vec2& p3, const glm::vec2& p4) {
    // Calculate directions
    float dx1 = p2.x - p1.x;
    float dy1 = p2.y - p1.y;
    float dx2 = p4.x - p3.x;
    float dy2 = p4.y - p3.y;

    // Calculate determinant
    float determinant = dx1 * dy2 - dy1 * dx2;

    // If lines are parallel or nearly parallel (avoid floating point issues)
    if (std::abs(determinant) < 0.0001f) return false;

    // Calculate parameters
    float t1 = ((p3.x - p1.x) * dy2 - (p3.y - p1.y) * dx2) / determinant;
    float t2 = ((p1.x - p3.x) * dy1 - (p1.y - p3.y) * dx1) / -determinant;

    // Check if intersection point is on both segments
    return (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1);
}

SelectionList GameMap::SelectRect(int startX, int startY, int endX, int endY, int z, GameCam* camera) {
    // Ensure start coordinates are smaller than end coordinates
 



    SelectionList list;

    auto p0 = SelectPoint(camera, startX, startY);
    auto p1 = SelectPoint(camera, endX, endY);

    if (p0.m_List.size() > 0 && p1.m_List.size() > 0) {
        int sY = std::min(p0.m_List[0].m_Y, p1.m_List[0].m_Y);
        int sX = std::min(p0.m_List[0].m_X, p1.m_List[0].m_X);
        int bX = std::max(p0.m_List[0].m_X, p1.m_List[0].m_X);
        int bY = std::max(p0.m_List[0].m_Y, p1.m_List[0].m_Y);

        for (int y = sY; y <= bY; y++) {
            for (int x = sX; x <= bX; x++) {

                if (x == sX || x == bX || y == sY || y == bY) {
                    Selection s1;
                    s1.m_X = x;
                    s1.m_Y = y;
                    s1.m_Z = 0;
                    list.m_List.push_back(s1);
                }
            }
        }
    }

    return list;

    // Display dimensions needed for coordinate conversion
    float midX = m_DW / 2.0f;
    float midY = m_DH / 2.0f;

    // Create the rectangle lines for intersection testing
    std::vector<std::pair<glm::vec2, glm::vec2>> rectLines = {
        // Top line
        {glm::vec2(startX, startY), glm::vec2(endX, startY)},
        // Right line
        {glm::vec2(endX, startY), glm::vec2(endX, endY)},
        // Bottom line
        {glm::vec2(endX, endY), glm::vec2(startX, endY)},
        // Left line
        {glm::vec2(startX, endY), glm::vec2(startX, startY)}
    };

    // Iterate through all tiles
    for (int y = 0; y < m_Height; y++) {
        for (int x = 0; x < m_Width; x++) {
            // Get the tile (may be nullptr)
            auto tile = GetTile(x, y, z);

            // Calculate tile position in screen space
            float drawX = x * m_TileWidth + m_TileWidth / 2;
            float drawY = y * m_TileHeight + m_TileHeight / 2;

            // Apply camera transformation (position, rotation, scale)
            drawX -= camera->GetPosition().x;
            drawY -= camera->GetPosition().y;

            float pX = drawX - midX;
            float pY = drawY - midY;

            float rot = camera->GetRotation().y;
            float scale = camera->GetPosition().z;

            auto renderPos = MathsOps::TransformCoord(glm::vec2(pX, pY), rot, scale);
            renderPos = glm::vec2(midX, midY) + renderPos;

            // Calculate tile corners in screen space
            float halfWidth = (m_TileWidth * scale) / 2.0f;
            float halfHeight = (m_TileHeight * scale) / 2.0f;

            std::vector<glm::vec2> tileCorners = {
                glm::vec2(-halfWidth, -halfHeight),
                glm::vec2(halfWidth, -halfHeight),
                glm::vec2(halfWidth, halfHeight),
                glm::vec2(-halfWidth, halfHeight)
            };

            // Rotate and position the corners
            for (auto& corner : tileCorners) {
                corner = MathsOps::TransformCoord(corner, rot, 1.0f);
                corner += renderPos;
            }

            // Check if this tile intersects with the rectangle outline
            bool intersects = false;

            // Check if any tile edge intersects with any rectangle edge
            for (int i = 0; i < 4; i++) {
                glm::vec2 tileStart = tileCorners[i];
                glm::vec2 tileEnd = tileCorners[(i + 1) % 4];

                for (const auto& rectLine : rectLines) {
                    // Line segment intersection test
                    if (LineSegmentsIntersect(tileStart, tileEnd, rectLine.first, rectLine.second)) {
                        intersects = true;
                        break;
                    }
                }
                if (intersects) break;
            }

            // If this tile intersects with the rectangle, add it to the selection
            if (intersects) {
                Selection sel;
                sel.m_X = x;
                sel.m_Y = y;
                sel.m_Z = z;
                sel.m_Tile = tile;
                sel.HasTile = (tile != nullptr);
                list.m_List.push_back(sel);
            }
        }
    }

    return list;
}SelectionList GameMap::SelectRectFilled(int startX, int startY, int endX, int endY, int z, GameCam* camera) {


    // Ensure start coordinates are smaller than end coordinates




    SelectionList list;

    auto p0 = SelectPoint(camera, startX, startY);
    auto p1 = SelectPoint(camera, endX, endY);

    if (p0.m_List.size() > 0 && p1.m_List.size() > 0) {
        int sY = std::min(p0.m_List[0].m_Y, p1.m_List[0].m_Y);
        int sX = std::min(p0.m_List[0].m_X, p1.m_List[0].m_X);
        int bX = std::max(p0.m_List[0].m_X, p1.m_List[0].m_X);
        int bY = std::max(p0.m_List[0].m_Y, p1.m_List[0].m_Y);

        for (int y = sY; y <= bY; y++) {
            for (int x = sX; x <= bX; x++) {

                Selection s1;
                s1.m_X = x;
                s1.m_Y = y;
                s1.m_Z = 0;
                list.m_List.push_back(s1);
            }
        }
    }

    return list;


    // Screen center coordinates
    float midX = m_DW / 2.0f;
    float midY = m_DH / 2.0f;

    // Define the rectangle corners in screen space
    std::vector<glm::vec2> rectScreenCorners = {
        glm::vec2(startX, startY),   // Top-left
        glm::vec2(endX, startY),     // Top-right
        glm::vec2(endX, endY),       // Bottom-right
        glm::vec2(startX, endY)      // Bottom-left
    };

    // Transform each corner from screen space to map local space (tile coordinates)
    std::vector<glm::vec2> rectLocalCorners;
    for (const auto& screenCorner : rectScreenCorners) {
        // Step 1: Translate relative to screen center
        glm::vec2 centered = screenCorner - glm::vec2(midX, midY);

        // Step 2: Apply inverse camera rotation
        float invRot = -camera->GetRotation().y;
        glm::vec2 rotated;
        rotated.x = centered.x * cos(invRot) - centered.y * sin(invRot);
        rotated.y = centered.x * sin(invRot) + centered.y * cos(invRot);

        // Step 3: Apply inverse camera scale
        float invScale = 1.0f / camera->GetPosition().z;
        glm::vec2 scaled = rotated * invScale;

        // Step 4: Add camera position to get world coordinates
        glm::vec2 worldPos = scaled + glm::vec2(midX, midY) +
            glm::vec2(camera->GetPosition().x, camera->GetPosition().y);

        // Step 5: Convert world coordinates to tile coordinates
        glm::vec2 tileCoord;
        tileCoord.x = worldPos.x / m_TileWidth;
        tileCoord.y = worldPos.y / m_TileHeight;

        rectLocalCorners.push_back(tileCoord);
    }

    // Find the axis-aligned bounding box of the transformed rectangle
    // This gives us the range of tiles we need to check
    float minTileX = FLT_MAX;
    float minTileY = FLT_MAX;
    float maxTileX = -FLT_MAX;
    float maxTileY = -FLT_MAX;

    for (const auto& corner : rectLocalCorners) {
        minTileX = std::min(minTileX, corner.x);
        minTileY = std::min(minTileY, corner.y);
        maxTileX = std::max(maxTileX, corner.x);
        maxTileY = std::max(maxTileY, corner.y);
    }

    // Expand slightly to ensure we catch edge tiles
    int startTileX = std::max(0, static_cast<int>(floor(minTileX - 0.01f)));
    int startTileY = std::max(0, static_cast<int>(floor(minTileY - 0.01f)));
    int endTileX = std::min(m_Width - 1, static_cast<int>(ceil(maxTileX + 0.01f)));
    int endTileY = std::min(m_Height - 1, static_cast<int>(ceil(maxTileY + 0.01f)));

    // Create a local-space polygon for the selection rectangle
    std::vector<glm::vec2> selectionPoly = rectLocalCorners;

    // Check each tile within the bounding box
    for (int y = startTileY; y <= endTileY; y++) {
        for (int x = startTileX; x <= endTileX; x++) {
            // Create a polygon for this tile in local space
            std::vector<glm::vec2> tilePolygon = {
                glm::vec2(x, y),           // Top-left
                glm::vec2(x + 1, y),       // Top-right
                glm::vec2(x + 1, y + 1),   // Bottom-right
                glm::vec2(x, y + 1)        // Bottom-left
            };

            // Check if this tile intersects with the transformed rectangle
            if (PolygonsIntersect(tilePolygon, selectionPoly)) {
                auto tile = GetTile(x, y, z);

                Selection sel;
                sel.m_X = x;
                sel.m_Y = y;
                sel.m_Z = z;
                sel.m_Tile = tile;
                sel.HasTile = (tile != nullptr);
                list.m_List.push_back(sel);
            }
        }
    }

    return list;
}


SelectionList GameMap::SelectLine(int startX, int startY, int endX, int endY, int z, GameCam* camera) {
    SelectionList list;

    float midX = m_DW / 2.0f;
    float midY = m_DH / 2.0f;

    // Line endpoints
    glm::vec2 lineStart(startX, startY);
    glm::vec2 lineEnd(endX, endY);

    // Iterate through all tiles
    for (int y = 0; y < m_Height; y++) {
        for (int x = 0; x < m_Width; x++) {
            // Get the tile (may be nullptr)
            auto tile = GetTile(x, y, z);

            // Calculate tile position in screen space
            float drawX = x * m_TileWidth + m_TileWidth / 2;
            float drawY = y * m_TileHeight + m_TileHeight / 2;

            // Apply camera transformation (position, rotation, scale)
            drawX -= camera->GetPosition().x;
            drawY -= camera->GetPosition().y;

            float pX = drawX - midX;
            float pY = drawY - midY;

            float rot = camera->GetRotation().y;
            float scale = camera->GetPosition().z;

            auto renderPos = MathsOps::TransformCoord(glm::vec2(pX, pY), rot, scale);
            renderPos = glm::vec2(midX, midY) + renderPos;

            // Calculate tile corners in screen space
            float halfWidth = (m_TileWidth * scale) / 2.0f;
            float halfHeight = (m_TileHeight * scale) / 2.0f;

            std::vector<glm::vec2> tileCorners = {
                glm::vec2(-halfWidth, -halfHeight),
                glm::vec2(halfWidth, -halfHeight),
                glm::vec2(halfWidth, halfHeight),
                glm::vec2(-halfWidth, halfHeight)
            };

            // Rotate and position the corners
            for (auto& corner : tileCorners) {
                corner = MathsOps::TransformCoord(corner, rot, 1.0f);
                corner += renderPos;
            }

            // Check if the line intersects with any edge of the tile
            bool intersects = false;
            for (int i = 0; i < 4; i++) {
                glm::vec2 tileStart = tileCorners[i];
                glm::vec2 tileEnd = tileCorners[(i + 1) % 4];

                if (LineSegmentsIntersect(tileStart, tileEnd, lineStart, lineEnd)) {
                    intersects = true;
                    break;
                }
            }

            // Also check if either end of the line is inside the tile
            if (!intersects) {
                if ((IsPointInTriangle(lineStart, tileCorners[0], tileCorners[1], tileCorners[2]) ||
                    IsPointInTriangle(lineStart, tileCorners[2], tileCorners[3], tileCorners[0])) ||
                    (IsPointInTriangle(lineEnd, tileCorners[0], tileCorners[1], tileCorners[2]) ||
                        IsPointInTriangle(lineEnd, tileCorners[2], tileCorners[3], tileCorners[0]))) {
                    intersects = true;
                }
            }

            // If this tile intersects with the line, add it to the selection
            if (intersects) {
                Selection sel;
                sel.m_X = x;
                sel.m_Y = y;
                sel.m_Z = z;
                sel.m_Tile = tile;
                sel.HasTile = (tile != nullptr);
                list.m_List.push_back(sel);
            }
        }
    }

    return list;
}

SelectionList GameMap::SelectOval(int startX, int startY, int endX, int endY, int z, GameCam* camera) {
    // Ensure start coordinates are smaller than end coordinates
    if (startX > endX) std::swap(startX, endX);
    if (startY > endY) std::swap(startY, endY);

    SelectionList list;

    float midX = m_DW / 2.0f;
    float midY = m_DH / 2.0f;

    // Calculate ellipse parameters
    float ellipseCenterX = (startX + endX) / 2.0f;
    float ellipseCenterY = (startY + endY) / 2.0f;
    float ellipseRadiusX = (endX - startX) / 2.0f;
    float ellipseRadiusY = (endY - startY) / 2.0f;

    // Special case: if either radius is 0, fall back to a line
    if (ellipseRadiusX < 1.0f || ellipseRadiusY < 1.0f) {
        return SelectLine(startX, startY, endX, endY, z, camera);
    }

    // Generate ellipse points (for better intersection testing)
    const int numSamples = 32; // More samples for better accuracy
    std::vector<glm::vec2> ellipsePoints;

    for (int i = 0; i < numSamples; i++) {
        float angle = 2.0f * M_PI * i / numSamples;
        float x = ellipseCenterX + ellipseRadiusX * std::cos(angle);
        float y = ellipseCenterY + ellipseRadiusY * std::sin(angle);
        ellipsePoints.push_back(glm::vec2(x, y));
    }

    // Iterate through all tiles
    for (int y = 0; y < m_Height; y++) {
        for (int x = 0; x < m_Width; x++) {
            // Get the tile (may be nullptr)
            auto tile = GetTile(x, y, z);

            // Calculate tile position in screen space
            float drawX = x * m_TileWidth + m_TileWidth / 2;
            float drawY = y * m_TileHeight + m_TileHeight / 2;

            // Apply camera transformation (position, rotation, scale)
            drawX -= camera->GetPosition().x;
            drawY -= camera->GetPosition().y;

            float pX = drawX - midX;
            float pY = drawY - midY;

            float rot = camera->GetRotation().y;
            float scale = camera->GetPosition().z;

            auto renderPos = MathsOps::TransformCoord(glm::vec2(pX, pY), rot, scale);
            renderPos = glm::vec2(midX, midY) + renderPos;

            // Calculate tile corners in screen space
            float halfWidth = (m_TileWidth * scale) / 2.0f;
            float halfHeight = (m_TileHeight * scale) / 2.0f;

            std::vector<glm::vec2> tileCorners = {
                glm::vec2(-halfWidth, -halfHeight),
                glm::vec2(halfWidth, -halfHeight),
                glm::vec2(halfWidth, halfHeight),
                glm::vec2(-halfWidth, halfHeight)
            };

            // Rotate and position the corners
            for (auto& corner : tileCorners) {
                corner = MathsOps::TransformCoord(corner, rot, 1.0f);
                corner += renderPos;
            }

            // Check if any corner is approximately on the ellipse
            bool onEllipse = false;
            for (const auto& corner : tileCorners) {
                // Calculate normalized distance from ellipse center
                float dx = (corner.x - ellipseCenterX) / ellipseRadiusX;
                float dy = (corner.y - ellipseCenterY) / ellipseRadiusY;
                float distSq = dx * dx + dy * dy;

                // Check if point is approximately on the ellipse
                if (std::abs(distSq - 1.0f) < 0.1f) {
                    onEllipse = true;
                    break;
                }
            }

            // Check if the tile edges intersect with the ellipse
            bool edgeIntersects = false;
            if (!onEllipse) {
                for (int i = 0; i < numSamples; i++) {
                    glm::vec2 start = ellipsePoints[i];
                    glm::vec2 end = ellipsePoints[(i + 1) % numSamples];

                    for (int j = 0; j < 4; j++) {
                        glm::vec2 tileStart = tileCorners[j];
                        glm::vec2 tileEnd = tileCorners[(j + 1) % 4];

                        if (LineSegmentsIntersect(tileStart, tileEnd, start, end)) {
                            edgeIntersects = true;
                            break;
                        }
                    }
                    if (edgeIntersects) break;
                }
            }

            // If this tile intersects with the ellipse outline, add it to the selection
            if (onEllipse || edgeIntersects) {
                Selection sel;
                sel.m_X = x;
                sel.m_Y = y;
                sel.m_Z = z;
                sel.m_Tile = tile;
                sel.HasTile = (tile != nullptr);
                list.m_List.push_back(sel);
            }
        }
    }

    return list;
}

SelectionList GameMap::SelectOvalFilled(int startX, int startY, int endX, int endY, int z, GameCam* camera) {
  

    SelectionList list;
//    return;
    float midX = m_DW / 2.0f;
    float midY = m_DH / 2.0f;

    // Calculate ellipse parameters
    float ellipseCenterX = (startX + endX) / 2.0f;
    float ellipseCenterY = (startY + endY) / 2.0f;
    float ellipseRadiusX = (endX - startX) / 2.0f;
    float ellipseRadiusY = (endY - startY) / 2.0f;

    // Special case: if either radius is 0, fall back to a line
    if (ellipseRadiusX < 1.0f || ellipseRadiusY < 1.0f) {
        return SelectLine(startX, startY, endX, endY, z, camera);
    }

    // Generate ellipse points (for better intersection testing)
    const int numSamples = 32; // More samples for better accuracy
    std::vector<glm::vec2> ellipsePoints;

    for (int i = 0; i < numSamples; i++) {
        float angle = 2.0f * M_PI * i / numSamples;
        float x = ellipseCenterX + ellipseRadiusX * std::cos(angle);
        float y = ellipseCenterY + ellipseRadiusY * std::sin(angle);
        ellipsePoints.push_back(glm::vec2(x, y));
    }

    // Iterate through all tiles
    for (int y = 0; y < m_Height; y++) {
        for (int x = 0; x < m_Width; x++) {
            // Get the tile (may be nullptr)
            auto tile = GetTile(x, y, z);

            // Calculate tile position in screen space
            float drawX = x * m_TileWidth + m_TileWidth / 2;
            float drawY = y * m_TileHeight + m_TileHeight / 2;

            // Apply camera transformation (position, rotation, scale)
            drawX -= camera->GetPosition().x;
            drawY -= camera->GetPosition().y;

            float pX = drawX - midX;
            float pY = drawY - midY;

            float rot = camera->GetRotation().y;
            float scale = camera->GetPosition().z;

            auto renderPos = MathsOps::TransformCoord(glm::vec2(pX, pY), rot, scale);
            renderPos = glm::vec2(midX, midY) + renderPos;

            // Calculate tile corners in screen space
            float halfWidth = (m_TileWidth * scale) / 2.0f;
            float halfHeight = (m_TileHeight * scale) / 2.0f;

            std::vector<glm::vec2> tileCorners = {
                glm::vec2(-halfWidth, -halfHeight),
                glm::vec2(halfWidth, -halfHeight),
                glm::vec2(halfWidth, halfHeight),
                glm::vec2(-halfWidth, halfHeight)
            };

            // Rotate and position the corners
            for (auto& corner : tileCorners) {
                corner = MathsOps::TransformCoord(corner, rot, 1.0f);
                corner += renderPos;
            }

            // Check if any corner is inside the ellipse
            bool anyCornerInside = false;
            for (const auto& corner : tileCorners) {
                // Calculate normalized distance from ellipse center
                float dx = (corner.x - ellipseCenterX) / ellipseRadiusX;
                float dy = (corner.y - ellipseCenterY) / ellipseRadiusY;
                float distSq = dx * dx + dy * dy;

                // Check if point is inside the ellipse
                if (distSq <= 1.0f) {
                    anyCornerInside = true;
                    break;
                }
            }

            // Check if ellipse center is inside the tile
            bool centerInside = IsPointInTriangle(glm::vec2(ellipseCenterX, ellipseCenterY),
                tileCorners[0], tileCorners[1], tileCorners[2]) ||
                IsPointInTriangle(glm::vec2(ellipseCenterX, ellipseCenterY),
                    tileCorners[2], tileCorners[3], tileCorners[0]);

            // Check for edge intersections
            bool edgeIntersects = false;
            if (!anyCornerInside && !centerInside) {
                for (int i = 0; i < numSamples; i++) {
                    glm::vec2 start = ellipsePoints[i];
                    glm::vec2 end = ellipsePoints[(i + 1) % numSamples];

                    for (int j = 0; j < 4; j++) {
                        glm::vec2 tileStart = tileCorners[j];
                        glm::vec2 tileEnd = tileCorners[(j + 1) % 4];

                        if (LineSegmentsIntersect(tileStart, tileEnd, start, end)) {
                            edgeIntersects = true;
                            break;
                        }
                    }
                    if (edgeIntersects) break;
                }
            }

            // If this tile is inside the ellipse or intersects with its boundary, add it to the selection
            if (anyCornerInside || centerInside || edgeIntersects) {
                Selection sel;
                sel.m_X = x;
                sel.m_Y = y;
                sel.m_Z = z;
                sel.m_Tile = tile;
                sel.HasTile = (tile != nullptr);
                list.m_List.push_back(sel);
            }
        }
    }

    return list;
}

bool GameMap::IsPointInPolygon(const glm::vec2& point, const std::array<glm::vec2, 4>& vertices) {
    bool inside = false;

    for (int i = 0, j = 3; i < 4; j = i++) {
        if (((vertices[i].y > point.y) != (vertices[j].y > point.y)) &&
            (point.x < (vertices[j].x - vertices[i].x) * (point.y - vertices[i].y) /
                (vertices[j].y - vertices[i].y) + vertices[i].x)) {
            inside = !inside;
        }
    }

    return inside;
}

bool GameMap::PolygonsIntersect(const std::vector<glm::vec2>& polyA, const std::vector<glm::vec2>& polyB) {
    // For each polygon, check each edge
    for (size_t i = 0; i < polyA.size() + polyB.size(); i++) {
        // Get two points that make an edge
        glm::vec2 p1, p2;
        if (i < polyA.size()) {
            p1 = polyA[i];
            p2 = polyA[(i + 1) % polyA.size()];
        }
        else {
            size_t j = i - polyA.size();
            p1 = polyB[j];
            p2 = polyB[(j + 1) % polyB.size()];
        }

        // Calculate the normal to this edge (perpendicular)
        glm::vec2 normal(-(p2.y - p1.y), p2.x - p1.x);

        // Normalize the normal vector
        float length = sqrt(normal.x * normal.x + normal.y * normal.y);
        normal.x /= length;
        normal.y /= length;

        // Project both polygons onto the normal
        float minA = FLT_MAX, maxA = -FLT_MAX;
        float minB = FLT_MAX, maxB = -FLT_MAX;

        // Project polygon A
        for (const auto& p : polyA) {
            float projection = normal.x * p.x + normal.y * p.y;
            minA = std::min(minA, projection);
            maxA = std::max(maxA, projection);
        }

        // Project polygon B
        for (const auto& p : polyB) {
            float projection = normal.x * p.x + normal.y * p.y;
            minB = std::min(minB, projection);
            maxB = std::max(maxB, projection);
        }

        // Check for separation
        if (maxA < minB || maxB < minA) {
            // Found a separating axis, so polygons don't intersect
            return false;
        }
    }

    // No separating axis found, polygons must intersect
    return true;
}

GameTile* m_BaseTile;


std::vector<Selection> GameMap::SFill(int x, int y, GameTile* base, std::vector<Selection> selects)
{

    for (auto s : m_Used) {

        if (s.m_X == x && s.m_Y == y) {
            return selects;
        }

    }

    if (x < 0 || y < 0 || x >= m_Width || y >= m_Height) {
        return selects;
    }

    auto tile = GetTile(x, y, 0);
    if (tile == base) {



        Selection s;
        s.m_X = x;
        s.m_Y = y;
        s.m_Z = 0;
        selects.push_back(s);

        m_Used.push_back(s);

        selects = SFill(x + 1, y, base, selects);
        selects = SFill(x - 1, y, base, selects);
        selects = SFill(x, y - 1, base, selects);
        selects = SFill(x, y + 1, base, selects);


    }
    else {
        return selects;
    }

    return selects;

}

SelectionList GameMap::SmartFill(int startX, int startY, GameCam* cam, GameTile* tile) {


    auto p = SelectPoint(cam,startX, startY);

    if (p.m_List.size() == 0) {
        return SelectionList();
    }
    m_BaseTile = p.m_List[0].m_Tile;

    std::vector<Selection> selects;

    m_Used.clear();

    selects = SFill(p.m_List[0].m_X, p.m_List[0].m_Y, m_BaseTile,selects);

    SelectionList rl;

    rl.m_List = selects;

    return rl;

    for (auto s : selects) {

      //  SetTile(s.m_X, s.m_Y, 0, tile);

    }


    int b = 5;

}

void GameMap::RenderAligned(GameCam* camera, Texture2D* image,glm::vec2 pos,glm::vec2 size,bool ir) {
    m_DW = FutureApp::m_Inst->GetWidth();
    m_DH = FutureApp::m_Inst->GetHeight();
    float midX = FutureApp::m_Inst->GetWidth() / 2.0f;
    float midY = FutureApp::m_Inst->GetHeight() / 2.0f;

    FutureApp::m_Inst->ClearZ();

    m_TestRender->Begin();

        float drawX = pos.x;
        float drawY = pos.y;

        drawX -= camera->GetPosition().x;

        drawY -= camera->GetPosition().y;


        float pX = drawX - midX;
        float pY = drawY - midY;

        float rot = camera->GetRotation().y;

        auto renderPos = MathsOps::TransformCoord(glm::vec2(pX, pY), rot, camera->GetPosition().z);

        renderPos = glm::vec2(midX, midY) +
            renderPos;

  
        if (ir) {
            rot = 0;
        }
        auto info = m_TestRender->Draw(renderPos, glm::vec2(size.x,size.y), glm::vec4(1, 1, 1, 1),image, rot, camera->GetPosition().z);
        //info->SetNormalTexture(frame->m_Normal);


        glm::vec4 ext(0, 0, 0, 0);



        info->SetExtra(ext);
        info->SetView(UIHelp::m_Scissor);

        m_TestRender->End();

    }

void GameMap::RenderGizmos(GameCam* cam) {

    for (auto l : m_Lights) {

        RenderAligned(cam, m_LightIcon, glm::vec2(l->GetPosition().x, l->GetPosition().y), glm::vec2(64, 64));

    }

}

GameObj* GameMap::MouseOver(GameCam* cam, glm::vec2 pos) {

    for (auto l : m_Lights) {

        if (IsWithin(cam, glm::vec2(l->GetPosition().x, l->GetPosition().y), glm::vec2(64, 64),pos)) {
            return l;
        }


    }
    return nullptr;

}

bool GameMap::IsWithin(GameCam* camera, glm::vec2 pos, glm::vec2 size, glm::vec2 mp) {

    float midX = m_DW / 2.0f;
    float midY = m_DH / 2.0f;




    float drawX = pos.x;// *m_TileWidth + m_TileWidth / 2;
    float drawY = pos.y;// *m_TileHeight + m_TileHeight / 2;

    drawX -= camera->GetPosition().x;

    drawY -= camera->GetPosition().y;


    float pX = drawX - midX;
    float pY = drawY - midY;

    float rot = camera->GetRotation().y;

    auto renderPos = MathsOps::TransformCoord(glm::vec2(pX, pY), rot, camera->GetPosition().z);

    renderPos = glm::vec2(midX, midY) +
        renderPos;


    //auto tile = GetTile(x, y, z);


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

    float scale = camera->GetPosition().z;

    auto v1 = MathsOps::TransformCoord(glm::vec2(tx0, ty0), rot, scale);
    auto v2 = MathsOps::TransformCoord(glm::vec2(tx1, ty1), rot, scale);
    auto v3 = MathsOps::TransformCoord(glm::vec2(tx2, ty2), rot, scale);
    auto v4 = MathsOps::TransformCoord(glm::vec2(tx3, ty3), rot, scale);


    v1 = glm::vec2(renderPos.x + v1.x, renderPos.y + v1.y);
    v2 = glm::vec2(renderPos.x + v2.x, renderPos.y + v2.y);
    v3 = glm::vec2(renderPos.x + v3.x, renderPos.y + v3.y);
    v4 = glm::vec2(renderPos.x + v4.x, renderPos.y + v4.y);

    if (IsPointInTriangle(glm::vec2(mp.x, mp.y), v1, v2, v3) || IsPointInTriangle(glm::vec2(mp.x, mp.y), v3, v4, v1)) {

        return true;
        //return list;

    }
}


glm::vec2 GameMap::MapPosition(GameCam* cam,glm::vec2 position) {

    m_DW = FutureApp::m_Inst->GetWidth();
    m_DH = FutureApp::m_Inst->GetHeight();
    float midX = FutureApp::m_Inst->GetWidth() / 2.0f;
    float midY = FutureApp::m_Inst->GetHeight() / 2.0f;

    m_TestRender->Begin();

    float drawX = position.x;
    float drawY = position.y;

    drawX -= cam->GetPosition().x;

    drawY -= cam->GetPosition().y;


    float pX = drawX - midX;
    float pY = drawY - midY;

    float rot = cam->GetRotation().y;

    auto renderPos = MathsOps::TransformCoord(glm::vec2(pX, pY), rot, cam->GetPosition().z);

    renderPos = glm::vec2(midX, midY) +
        renderPos;

//    auto info = m_TestRender->Draw(renderPos, glm::vec2(size.x, size.y), glm::vec4(1, 1, 1, 1), image, rot, camera->GetPosition().z);

    return renderPos;

}

bool VectorContainsTile(const std::vector<GameTile*>& tiles, GameTile* tileToCheck) {
    // Use std::find to look for the pointer in the vector
    // It returns an iterator to the element if found, or tiles.end() if not found
    return std::find(tiles.begin(), tiles.end(), tileToCheck) != tiles.end();
}
int FindTileIndex(const std::vector<GameTile*>& tiles, GameTile* tileToFind) {
    // Find the iterator to the element
    auto it = std::find(tiles.begin(), tiles.end(), tileToFind);

    // If the element was found, calculate its index
    if (it != tiles.end()) {
        return static_cast<int>(std::distance(tiles.begin(), it));
    }

    // Return -1 if the element wasn't found
    return -1;
}

void GameMap::SaveMap(std::string file) {
    FFile* f = new FFile(file, false);

    f->writeInt(m_Width);
    f->writeInt(m_Height);
    f->writeInt(m_Depth);
    f->writeInt(m_TileWidth);
    f->writeInt(m_TileHeight);

    auto tiles = GatherTiles();

    f->writeInt(tiles.size());
    for (int i = 0; i < tiles.size(); i++) {

        auto tile = tiles[i];
        tile->Write(f);

    }

    for (int z = 0; z < m_Depth; z++) {
        for (int y = 0; y < m_Height; y++) {
            for (int x = 0; x < m_Width; x++) {
                auto tile = GetTile(x, y, z);
                if (tile != nullptr) {
                    //		tile->Write(f);
                    int index = FindTileIndex(tiles, tile);
                    f->writeInt(index);
                }
                else {
                    f->writeInt(-1);
                }
            }
        }
    }

    f->writeInt(m_Lights.size());
    for (int i = 0; i < m_Lights.size(); i++) {
        auto light = m_Lights[i];
        light->Write(f);
    };

    f->close();

}

std::vector<GameTile*> GameMap::GatherTiles() {

    std::vector<GameTile*> tiles;

    for (int z = 0; z < m_Depth; z++) {
        for (int y = 0; y < m_Height; y++) {
            for (int x = 0; x < m_Width; x++) {

                auto tile = GetTile(x, y, z);
                if (tile != nullptr) {

                    if (!VectorContainsTile(tiles, tile)) {

						tiles.push_back(tile);

                    }

                }

            }
        }
    }
    return tiles;
}

void GameMap::OpenMap(std::string path) {

    FFile* f = new FFile(path, true);

    m_Width = f->readInt();
    m_Height = f->readInt();
    m_Depth = f->readInt();
    m_TileWidth = f->readInt();
    m_TileHeight = f->readInt();

    int tilesCount = f->readInt();

    std::vector<GameTile*> tiles;

    for (int i = 0; i < tilesCount; i++) {
        auto tile = new GameTile();
        tile->Read(f);
        tiles.push_back(tile);
    }

    for (int z = 0; z < m_Depth; z++) {
        for (int y = 0; y < m_Height; y++) {
            for (int x = 0; x < m_Width; x++) {
                int index = f->readInt();
                if (index != -1) {
                    auto tile = tiles[index];
                    SetTile(x, y, z, tile);
                }
                else {
                    SetTile(x, y, z, nullptr);
                }
            }
        }
    }

    int lightCount = f->readInt();
    for (int i = 0; i < lightCount; i++) {
        auto light = new GameLight();
        light->Read(f);
        m_Lights.push_back(light);
    }


    f->close();

}