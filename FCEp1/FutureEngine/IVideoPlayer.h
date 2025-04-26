#pragma once
#include "IControl.h"
#include <string>

class GameVideo;

class IVideoPlayer :
    public IControl
{
public:

    IVideoPlayer(std::string video, glm::vec2 position, glm::vec2 size);
    IVideoPlayer(glm::vec2 position, glm::vec2 size);
    void SetVideo(std::string path);
    void Play();
    void Pause();
    void Resume();
    void Update(float delta) override;
    void Render() override;
    void LoadSubtitles(std::string srtPath = "");
    float Position();
    bool Finished();
private:

    GameVideo* m_Video;

};

