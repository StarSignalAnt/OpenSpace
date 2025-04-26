#pragma once
#include <vector>

class DrawInfo;
class Texture2D;
class MeshBuffer;

class DrawList
{
public:

	void Add(DrawInfo* info);
	void SetTexture(Texture2D* texture);
	Texture2D* GetTexture();
	std::vector<DrawInfo*>& GetList() { return m_DrawList; }
	void Free();
	void SetMesh(MeshBuffer* buffer)
	{
		m_PreMesh = buffer;
	}
	MeshBuffer* GetMesh()
	{
		return m_PreMesh;
	}

private:

	std::vector<DrawInfo*> m_DrawList;
	Texture2D* m_Texture;
	MeshBuffer* m_PreMesh;

};

