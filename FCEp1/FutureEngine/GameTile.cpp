#include "GameTile.h"
#include "Texture2D.h"
#include "BinaryFile.h"

Texture2D* GameTile::m_BlankNormal = nullptr;

GameTile::GameTile() {

	if (m_BlankNormal == nullptr) {
		
		m_BlankNormal = new Texture2D("engine/maps/blanknormal.png");
	}

}

void GameTile::AddFrame(Texture2D* colorFrame, Texture2D* normalFrame)
{
	if (normalFrame == nullptr) {
		normalFrame = m_BlankNormal;
	}
	m_ColorFrames.push_back(colorFrame);
	m_NormalFrames.push_back(normalFrame);
}

std::vector<Texture2D*> GameTile::GetFrame(int index)
{
	std::vector<Texture2D*> frames;
	frames.push_back(m_ColorFrames[index]);
	frames.push_back(m_NormalFrames[index]);
	return frames;
}


void GameTile::SetColorFrame(Texture2D* color, int index) {

	m_ColorFrames[index] = color;

}

void GameTile::SetNormalFrame(Texture2D* normal, int index) {

	m_NormalFrames[index] = normal;

}


void GameTile::Write(FFile* f) {

	f->writeInt(m_ColorFrames.size());
	for (int i = 0; i < m_ColorFrames.size(); i++) {
		f->writeString(m_ColorFrames[i]->GetPath());
		f->writeString(m_NormalFrames[i]->GetPath());
	}
	f->writeString(m_Name);
	f->writeBool(m_CastShadow);
	f->writeBool(m_ReceivesShadow);
	f->writeBool(m_ReceivesLight);


}

void GameTile::Read(FFile* f) {

	int count = f->readInt();
	for (int i = 0; i < count; i++) {
		std::string colorPath = f->readString();
		std::string normalPath = f->readString();
		Texture2D* colorFrame = new Texture2D(colorPath);
		Texture2D* normalFrame = new Texture2D(normalPath);
		AddFrame(colorFrame, normalFrame);
	}
	m_Name = f->readString();
	m_CastShadow = f->readBool();
	m_ReceivesShadow = f->readBool();
	m_ReceivesLight = f->readBool();

}