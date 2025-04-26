#pragma once
#include <vector>
#include <string>
#include "SmartDraw.h"

class GameTile;
class GameCam;
class GameLight;
class GameSprite;
class ShaderModule;
class RenderTarget2D;
class Texture2D;
class GameObj;
class GPMap2D;

struct Selection {

	GameTile* m_Tile;
	int m_X, m_Y, m_Z;
	bool HasTile;

};

struct SelectionList {

	std::vector<Selection> m_List;

};


class GameMap
{
public:

	GameMap(int width,int height,int depth,int tileWidth,int tileHeight) : m_Width(width), m_Height(height), m_Depth(depth), m_TileWidth(tileWidth), m_TileHeight(tileHeight), m_Tiles(width*height*depth),m_Highlights(width*height) {
	 
		m_TileRenderer = new SmartDraw;
		m_TestRender = new SmartDraw;
		InitMap();

    }
	std::vector<Selection> SFill(int x, int y, GameTile* base, std::vector<Selection> selects);

	SelectionList Select(int x, int y, int z)
	{
		auto tile = GetTile(x, y, z);
		Selection selection;
		if (tile != nullptr) {
			selection.HasTile = true;
		}
		else {
			selection.HasTile = false;
		}
		selection.m_X = x;
		selection.m_Y = y;
		selection.m_Z = z;
		selection.m_Tile = tile;
		SelectionList list;
		list.m_List.push_back(selection);
		
		return list;

	}

	SelectionList SelectPoint(GameCam* cam, int x, int y);
	

	void InitMap();

	int Index(int x, int y, int z) const {
		return x + m_Width * (y + m_Height * z);
	}
    void SetTile(int x, int y, int z, GameTile* tile) {
        if (x >= 0 && x < m_Width && y >= 0 && y < m_Height && z >= 0 && z < m_Depth) {
            m_Tiles[Index(x, y, z)] = tile;
        }
    }

	void SetHighlight(int x, int y, bool hl)
	{
		m_Highlights[y * m_Width + x] = hl;

	}

	bool GetHighlight(int x, int y) {

		return m_Highlights[y * m_Width + x];

	}


    // Get a tile at (x, y, z)
    GameTile* GetTile(int x, int y, int z) const {
        if (x >= 0 && x < m_Width && y >= 0 && y < m_Height && z >= 0 && z < m_Depth) {
            return m_Tiles[Index(x, y, z)];
        }
        return nullptr; // Return default tile if out of bounds
    }

	void Fill(GameTile* tile,int z) {
		
			for (int y = 0; y < m_Height; y++) {
				for (int x = 0; x < m_Width; x++) {
					SetTile(x, y, z, tile);
				}
			}
		
	}

	void FIllBlock(GameTile* tile, int x, int y, int w, int h, int z);

	void AddLight(GameLight* light);

	void RenderMap(GameCam* camera);
	void RenderGrid(GameCam* camera);
	void RenderShadowMap();
	void RenderAligned(GameCam* camera, Texture2D* image, glm::vec2 pos, glm::vec2 size, bool inhibitrot = true);
	glm::vec2 MapPosition(GameCam* cam,glm::vec2 position);
	void AddSprite(GameSprite* sprite) {
		m_Sprites.push_back(sprite);
	}
	void ClearHighlights();

	SelectionList SelectRect(int startX, int startY, int endX, int endY, int z, GameCam* camera);
	SelectionList SelectRectFilled(int startX, int startY, int endX, int endY, int z, GameCam* camera);
	SelectionList SelectLine(int startX, int startY, int endX, int endY, int z, GameCam* camera);
	SelectionList SelectOval(int startX, int startY, int endX, int endY, int z, GameCam* camera);
	SelectionList SelectOvalFilled(int startX, int startY, int endX, int endY, int z, GameCam* camera);
	SelectionList SmartFill(int startX, int startY, GameCam* cam, GameTile* tile);
	// Helper function for line segment intersection
	bool LineSegmentsIntersect(const glm::vec2& p1, const glm::vec2& p2,
		const glm::vec2& p3, const glm::vec2& p4);
	bool IsPointInPolygon(const glm::vec2& point, const std::array<glm::vec2, 4>& vertices);
	bool PolygonsIntersect(const std::vector<glm::vec2>& polyA, const std::vector<glm::vec2>& polyB);
	std::vector<GameLight*> GetLights() {
		return m_Lights;
	}
	void RenderGizmos(GameCam* cam);
	GameObj* MouseOver(GameCam* cam, glm::vec2 pos);

	bool IsWithin(GameCam* cam, glm::vec2 pos, glm::vec2 size,glm::vec2 mp);

	void SaveMap(std::string path);
	void OpenMap(std::string path);
	std::vector<GameTile*> GatherTiles();

private:

	GPMap2D* m_M2D;
	int m_Width, m_Height, m_Depth;
	int m_TileWidth, m_TileHeight;
	std::vector<GameTile*> m_Tiles;
	std::vector < bool > m_Highlights;
	SmartDraw* m_TileRenderer;
	SmartDraw* m_TestRender;
	SmartDraw* m_ShadowRenderer;
	ShaderModule* m_DrawLit;
	ShaderModule* m_ShadowWrite;
	std::vector<GameLight*> m_Lights;
	RenderTarget2D* m_ShadowRT;
	int m_ShadowMapSize = 1024;
	std::vector<GameSprite*> m_Sprites;
	Texture2D* m_GridTex;
	Texture2D* m_SelectTex;
	int m_DW, m_DH;
	Texture2D* m_LightIcon;
	Texture2D* m_LeftRight;
	Texture2D* m_UpDown;
	std::vector<Selection> m_Used;
};

