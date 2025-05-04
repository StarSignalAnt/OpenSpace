#pragma once
#include <string>
#include <vector>

class NodeEntity;
class NodeSkeletalEntity;

class Importer
{
public:

	static NodeEntity* ImportEntity(std::string path);
	static NodeSkeletalEntity* ImportSkeletal(std::string path);

};

