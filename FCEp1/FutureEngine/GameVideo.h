#pragma once
#include <string>
#include <vector>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h> // Audio resampling
}
#include <AL/al.h>      // Core OpenAL API
#include <AL/alc.h>     // Context and device management
#include <AL/alext.h> 
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libavutil/avconfig.h>
class Texture2D;

struct Frame {

    Texture2D* Image = nullptr;
	double TimeStamp;
    std::string Subtitle = "";
    uint8_t* rgb = nullptr;


};


struct Subtitle {
    double startTime;
    double endTime;
    std::string text;
};

class GameVideo
{
public:

	GameVideo(std::string path);
    void LoadSubtitles(std::string srtPath = "");
    
	~GameVideo();
    void Play();
    void Update();
    Frame GetFrame();
    double GetCurrentFrameTimestamp() const;
    void Pause();
    void Resume();
    float GetPosition();
    float GetLength();
    bool IsFinished();
    // Add this private member variable to GameVideo.h in the private section

private:

    Texture2D* m_PrevTex = nullptr;
    Frame m_PrevFrame;
    uint8_t* prergb = nullptr;
    int p_up = 0;
    double parseTime(const std::string& timeStr);
    double m_TotalDuration = 0.0;
	std::string m_Path;
    std::vector<Subtitle> m_Subtitles;
    AVFormatContext* formatCtx = nullptr;
    AVCodecContext* videoCodecCtx = nullptr;
    AVCodecContext* audioCodecCtx = nullptr;
    AVFrame* videoFrame = nullptr;
    AVFrame* audioFrame = nullptr;
    AVPacket packet;
    int videoStreamIndex = -1;
    int audioStreamIndex = -1;
    ALCdevice* device;
    ALCcontext* context;
    ALuint source;
    Texture2D* m_CurrentFrame = nullptr;

	std::vector<Frame*> m_Frames;
    double m_CurrentFrameTimestamp; 

    bool isPlaying = false;
    double frameDelay = 1.0 / 30.0;  // For 30 FPS video, adjust as needed
    double lastFrameTime = 0.0;

    double StartTime = 0.0f;
    int StartClock = 0;

    SwsContext* swsContext = nullptr;
    bool m_NoAudio = false;
};

