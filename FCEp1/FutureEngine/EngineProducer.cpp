#include "EngineProducer.h"
#include "MaterialBasic3D.h"
#include "MaterialDepth3D.h"
#include "MaterialDepthS3D.h"
#include "MaterialPBR3D.h"
#include "MaterialPBRS3D.h"

EngineProducer* EngineProducer::m_This = nullptr;

EngineProducer::EngineProducer() {


	m_This = this;
	auto mat_basic = new MaterialBasic3D;
	auto mat_depth = new MaterialDepth3D;
	auto mat_depths = new MaterialDepthS3D;
	auto mat_pbr = new MaterialPBR3D;
	auto mat_pbrs = new MaterialPBRS3D;

	Register(mat_basic, "MaterialBasic");
	Register(mat_depth, "MaterialDepth");
	Register(mat_depths, "MaterialDepths");
	Register(mat_pbr, "MaterialPBR");
	Register(mat_pbrs, "MaterialPBRS");

}

void EngineProducer::Register(RenderMaterial* material, std::string name) {

	m_Materials[name] = material;

}

RenderMaterial* EngineProducer::ProduceMaterial(std::string name) {

	return m_Materials[name]->Clone();

}