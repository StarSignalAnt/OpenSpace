#include "GameLight.h"
#include "BinaryFile.h"

void GameLight::SetRange(float range) {

	m_Range = range;
	

}

float GameLight::GetRange()
{

	return m_Range;

}

void GameLight::Write(FFile* f) {
	f->writeVec3(m_Position);
	f->writeVec3(m_Diffuse);
	f->writeFloat(m_Range);

	//	f->writeInt(m_Diffuse);
//	f->writeInt(m_Range);
}


void GameLight::Read(FFile* f) {
	m_Position = f->readVec3();
	m_Diffuse = f->readVec3();
	m_Range = f->readFloat();

	//f->Read(m_Diffuse);
	//f->Read(m_Range);

}
