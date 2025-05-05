#pragma once
#include <map>
#include <string>

class RenderMaterial;

class EngineProducer
{
public:

	EngineProducer();
	void Register(RenderMaterial* material, std::string name);
	RenderMaterial* ProduceMaterial(std::string name);

	static EngineProducer* m_This;

private:

	std::map<std::string,RenderMaterial*> m_Materials;

};

