#include "IVideoPlayer.h"
#include "GameVideo.h"
#include "UIHelp.h"
#include "FutureApp.h"

IVideoPlayer::IVideoPlayer(std::string path, glm::vec2 position, glm::vec2 size) {

	m_Text = path;
	m_Position = position;
	m_Size = size;

}

IVideoPlayer::IVideoPlayer(glm::vec2 position, glm::vec2 size) {

	m_Position = position;
	m_Size = size;

}

void IVideoPlayer::Play() {

	m_Video = new GameVideo(m_Text);
	m_Video->Play();

}

void IVideoPlayer::Update(float delta) {

	m_Video->Update();

}

void IVideoPlayer::Render() {

	auto frame = m_Video->GetFrame();

	if (frame.Image != nullptr) {
		UIHelp::DrawImage(GetRenderPosition(), m_Size, frame.Image, m_Color);
	}

	float width = UIHelp::StrWidth(frame.Subtitle, 3.0f);



	UIHelp::DrawText(glm::vec2(FutureApp::m_Inst->GetWidth()/2-width/2,FutureApp::m_Inst->GetHeight()-UIHelp::StrHeight(frame.Subtitle)-64), frame.Subtitle, glm::vec4(1, 1, 1, 1.0f), 3.0f);

}

void IVideoPlayer::LoadSubtitles(std::string srtPath)
{

	m_Video->LoadSubtitles(srtPath);

}

float IVideoPlayer::Position() {

	return m_Video->GetPosition();

}

bool IVideoPlayer::Finished() {

	return m_Video->IsFinished();

}