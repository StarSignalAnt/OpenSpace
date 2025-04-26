#include "GameVideo.h"
#include <iostream>
#include "Texture2D.h"
#include <fstream>
#include <sstream>

GameVideo::GameVideo(std::string path)
{
	// Load video from path
	m_Path = path;
    avformat_network_init();
    m_CurrentFrameTimestamp = -1.0;  // Use -1.0 to indicate "not set yet"

    // Allocate format context
    formatCtx = avformat_alloc_context();
    if (!formatCtx) {
        std::cerr << "Failed to allocate AVFormatContext!" << std::endl;
        return;
    }

    // Open the video file
    if (avformat_open_input(&formatCtx, path.c_str(), nullptr, nullptr) != 0) {
        std::cerr << "Failed to open video file: " << path << std::endl;
        avformat_free_context(formatCtx);
        formatCtx = nullptr;
        return;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(formatCtx, nullptr) < 0) {
        std::cerr << "Failed to retrieve stream info!" << std::endl;
        avformat_close_input(&formatCtx);
        return;
    }

    if (formatCtx->duration != AV_NOPTS_VALUE) {
        m_TotalDuration = formatCtx->duration / (double)AV_TIME_BASE;
        std::cout << "Video duration: " << m_TotalDuration << " seconds" << std::endl;
    }
    else {
        // If the container doesn't have the duration, try to get it from the video stream
        if (videoStreamIndex >= 0) {
            AVStream* videoStream = formatCtx->streams[videoStreamIndex];
            if (videoStream->duration != AV_NOPTS_VALUE) {
                m_TotalDuration = videoStream->duration * av_q2d(videoStream->time_base);
                std::cout << "Video duration (from stream): " << m_TotalDuration << " seconds" << std::endl;
            }
            else {
                std::cerr << "Could not determine video duration!" << std::endl;
                m_TotalDuration = 0.0;
            }
        }
    }

    // Print video file info
    std::cout << "Opened video: " << path << std::endl;
    av_dump_format(formatCtx, 0, path.c_str(), 0);
    videoStreamIndex = -1;
    audioStreamIndex = -1;
    m_NoAudio = true;

    for (unsigned int i = 0; i < formatCtx->nb_streams; ++i) {
        AVCodecParameters* codecParams = formatCtx->streams[i]->codecpar;
        if (codecParams->codec_type == AVMEDIA_TYPE_VIDEO && videoStreamIndex == -1) {
            videoStreamIndex = i;
            const AVCodec* videoCodec = avcodec_find_decoder(codecParams->codec_id);
            videoCodecCtx = avcodec_alloc_context3(videoCodec);
            if (avcodec_parameters_to_context(videoCodecCtx, codecParams) < 0) {
                std::cerr << "Failed to copy codec parameters for video!" << std::endl;
            }
            avcodec_open2(videoCodecCtx, videoCodec, nullptr);
        }
        else if (codecParams->codec_type == AVMEDIA_TYPE_AUDIO && audioStreamIndex == -1) {
            audioStreamIndex = i;
            const AVCodec* audioCodec = avcodec_find_decoder(codecParams->codec_id);
            audioCodecCtx = avcodec_alloc_context3(audioCodec);
            if (avcodec_parameters_to_context(audioCodecCtx, codecParams) < 0) {
                std::cerr << "Failed to copy codec parameters for audio!" << std::endl;
            }
            avcodec_open2(audioCodecCtx, audioCodec, nullptr);
            m_NoAudio = false;
        }
        else {
           
        }
    }

    // Allocate frames
    videoFrame = av_frame_alloc();
    audioFrame = av_frame_alloc();
    av_packet_unref(&packet);

    // Initialize the software scaler (for converting video frames to RGB)
    swsContext = sws_getContext(videoCodecCtx->width, videoCodecCtx->height, videoCodecCtx->pix_fmt,
        videoCodecCtx->width, videoCodecCtx->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, nullptr, nullptr, nullptr);
     device = alcOpenDevice(nullptr);  // Use default device
    if (!device) {
        std::cerr << "Failed to open OpenAL device" << std::endl;
        return;
    }

     context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);

}

float getSourceTime(ALuint source) {
    float time = 0.0f;

    // Get the current playback time in seconds
    alGetSourcef(source, AL_SEC_OFFSET, &time);

    // Check for OpenAL errors
    ALenum alError = alGetError();
    if (alError != AL_NO_ERROR) {
        std::cerr << "OpenAL error getting source time: " << alError << std::endl;
        return -1.0f; // Return an invalid time if there's an error
    }

    return time;
}

void GameVideo::Play() {

    isPlaying = true;  // Start playback
    StartClock = clock();
   // lastFrameTime = av_gettime() / 1000000.0; // Current time in seconds
   // ALuint source;
    alGenSources(1, &source);
   // alSourcei(source, AL_BUFFER, buffer);
    alSourcePlay(source);
}
void GameVideo::Update() {
    if (!isPlaying) return;

    int time = clock();
    int diff = time - StartClock;
    float ctime = ((float)diff) / (1000.0f);

    if (time < p_up) {
        return;
    }
    p_up = time + 5;

    if (ctime >= m_TotalDuration) {

        isPlaying = false;

        return;


    };

    // Read a single packet from the input file
    if (av_read_frame(formatCtx, &packet) < 0) {
        // End of file or error
        return;
    }

  //  if (m_Frames.size() > 250) return;
    // Check if it's a video packet
    if (packet.stream_index == videoStreamIndex) {
        // Send the packet to the video decoder
        int ret = avcodec_send_packet(videoCodecCtx, &packet);
        if (ret >= 0) {
            // Receive a decoded frame from the video codec
            ret = avcodec_receive_frame(videoCodecCtx, videoFrame);
            if (ret >= 0) {

                AVStream* videoStream = formatCtx->streams[videoStreamIndex];

                // Get the presentation timestamp (PTS) and convert to seconds
                // If PTS is invalid, try to use DTS (decoding timestamp) instead
                int64_t timestamp = videoFrame->pts != AV_NOPTS_VALUE ? videoFrame->pts : videoFrame->pkt_dts;

                if (timestamp != AV_NOPTS_VALUE) {
                    // Convert timestamp to seconds using the stream's time_base
                    m_CurrentFrameTimestamp = timestamp * av_q2d(videoStream->time_base);
                }
                else {
                    // If no valid timestamp, use a fallback (could be previous timestamp + frame duration)
                    double frameRate = av_q2d(videoStream->r_frame_rate);
                    double frameDuration = frameRate > 0 ? 1.0 / frameRate : 0.04; // Default to 25fps if unknown

                    // If we have a previous timestamp, increment it
                    // Otherwise start from 0
                    if (m_CurrentFrameTimestamp >= 0) {
                        m_CurrentFrameTimestamp += frameDuration;
                    }
                    else {
                        m_CurrentFrameTimestamp = 0.0;
                    }
                }


                // Allocate the RGB buffer with correct size
                int rgbBufferSize = videoCodecCtx->width * videoCodecCtx->height * 3;
                uint8_t* rgbBuffer = new uint8_t[rgbBufferSize];

                // Set up the correct stride for the destination buffer
                int rgbStride[1] = { videoCodecCtx->width * 3 };
                uint8_t* rgbData[1] = { rgbBuffer };

                // Convert the video frame to RGB
                sws_scale(swsContext, videoFrame->data, videoFrame->linesize,
                    0, videoCodecCtx->height,
                    rgbData, rgbStride);

                // Create a new texture with the RGB data
                auto img = new Texture2D(videoCodecCtx->width, videoCodecCtx->height, rgbBuffer, 3);

                delete[] rgbBuffer;
            
                Frame* frame = new Frame;
                frame->Image = img;
                frame->TimeStamp = GetCurrentFrameTimestamp();

                for (auto sub : m_Subtitles) {


                    int b = 5;
               }

				m_Frames.push_back(frame);



                // Clean up the RGB buffer after use
                //delete rgbBuffer;

            }
        }
    }
    // Check if it's an audio packet
    else if (packet.stream_index == audioStreamIndex) {
        // Send the packet to the audio decoder
        int ret = avcodec_send_packet(audioCodecCtx, &packet);
        if (ret >= 0) {
            // Receive a decoded audio frame
            ret = avcodec_receive_frame(audioCodecCtx, audioFrame);
            if (ret >= 0) {
                ALenum format;
                int channels = audioFrame->ch_layout.nb_channels;

                // Determine the OpenAL format based on sample format and channels
                if (channels == 2) {
                    // Format is stereo, so we need to interleave the channels
                    format = AL_FORMAT_STEREO_FLOAT32;

                    // Number of samples per channel
                    int numSamples = audioFrame->nb_samples;

                    // Allocate a buffer for the interleaved data
                    float* interleavedData = new float[numSamples * channels];

                    // Get the left and right channel data from the planar format
                    float* leftChannel = (float*)audioFrame->data[0];
                    float* rightChannel = (float*)audioFrame->data[1];

                    // Interleave the data
                    for (int i = 0; i < numSamples; ++i) {
                        interleavedData[2 * i] = leftChannel[i];   // Left channel
                        interleavedData[2 * i + 1] = rightChannel[i]; // Right channel
                    }

                    // Calculate the data size for the interleaved data
                    int dataSize = numSamples * channels * sizeof(float);

                    // Create an OpenAL buffer and fill it with the interleaved data
                    ALuint buffer;
                    alGenBuffers(1, &buffer);
                    alBufferData(buffer, format, interleavedData, dataSize, audioFrame->sample_rate);

                    // Check for errors
                    ALenum alError = alGetError();
                    if (alError != AL_NO_ERROR) {
                        std::cerr << "OpenAL error loading buffer data: " << alError << std::endl;
                        delete[] interleavedData;  // Clean up allocated memory
                        return;
                    }

                    // Queue the buffer to the source
                    alSourceQueueBuffers(source, 1, &buffer);

                    // Check if the source is playing, start it if not
                    ALint sourceState;
                    alGetSourcei(source, AL_SOURCE_STATE, &sourceState);
                    if (sourceState != AL_PLAYING) {
                        alSourcePlay(source);

                        // Check for errors
                        alError = alGetError();
                        if (alError != AL_NO_ERROR) {
                            std::cerr << "OpenAL error playing source: " << alError << std::endl;
                        }
                    }

                    // Clean up allocated memory
                    delete[] interleavedData;
                }
                else {
                    std::cerr << "Unsupported channel count: " << channels << std::endl;
                    return;
                }

            }

        }
    }
    // Unreference the packet after processing it (important to avoid memory leaks)
    av_packet_unref(&packet);
}
Frame GameVideo::GetFrame() {
    if (!isPlaying || m_Frames.empty()) return Frame();

    // Get the current playback time
    double currentTime;// = getSourceTime(source);
    if (m_NoAudio) {

        int time = clock();
        int diff = time - StartClock;
        float ctime = ((float)diff) / (1000.0f);


        currentTime = ctime;
    }
    else {
        currentTime = getSourceTime(source);
    }
        

    // Find the appropriate subtitle for the current time
    std::string currentSubtitle;
    for (const auto& subtitle : m_Subtitles) {
        if (currentTime >= subtitle.startTime && currentTime <= subtitle.endTime) {
            currentSubtitle = subtitle.text;
            break;
        }
    }

    // Find the appropriate frame for the current time
    for (auto it = m_Frames.begin(); it != m_Frames.end(); ) {
        Frame* frame = *it;

        if (frame->TimeStamp > currentTime) {
            // This frame is for the future, so use it
            frame->Subtitle = currentSubtitle;  // Update the subtitle text
            return *frame;
        }
        else {
            // This frame is in the past, we can remove it after using
            frame->Image->Free();
            it = m_Frames.erase(it);
        }
    }

    return Frame();  // No suitable frame found
}
double GameVideo::GetCurrentFrameTimestamp() const {
    return m_CurrentFrameTimestamp;
}


GameVideo::~GameVideo()
{
	// Close the video file
    if (formatCtx) {
        avformat_close_input(&formatCtx);
    }
    avformat_network_deinit();
}

void GameVideo::Pause() {
	isPlaying = false;
	alSourcePause(source);
}

void GameVideo::Resume() {
    isPlaying = true;
	alSourcePlay(source);
}

void GameVideo::LoadSubtitles(std::string srtPath) {
    std::ifstream file(srtPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open subtitle file: " << srtPath << std::endl;
        return;
    }

    std::string line;
    int subtitleNumber = 0;
    double startTime = -1.0, endTime = -1.0;
    std::string subtitleText;

    enum ParseState { NUMBER, TIMING, TEXT, BLANK };
    ParseState state = NUMBER;

    while (std::getline(file, line)) {
        // Remove BOM if present at the beginning of the file
        if (line.length() >= 3 &&
            static_cast<unsigned char>(line[0]) == 0xEF &&
            static_cast<unsigned char>(line[1]) == 0xBB &&
            static_cast<unsigned char>(line[2]) == 0xBF) {
            line = line.substr(3);
        }

        // Remove carriage returns that might be present in Windows files
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        switch (state) {
        case NUMBER:
            // Try to parse as a subtitle number
            try {
                subtitleNumber = std::stoi(line);
                state = TIMING;
            }
            catch (const std::exception&) {
                // If it's not a number, skip this line
                std::cerr << "Unexpected line (expected subtitle number): " << line << std::endl;
            }
            break;

        case TIMING:
            // Parse timing line
            if (line.find("-->") != std::string::npos) {
                size_t arrowPos = line.find("-->");
                std::string startStr = line.substr(0, arrowPos);
                std::string endStr = line.substr(arrowPos + 3);

                // Trim whitespace
                startStr.erase(0, startStr.find_first_not_of(" \t"));
                startStr.erase(startStr.find_last_not_of(" \t") + 1);
                endStr.erase(0, endStr.find_first_not_of(" \t"));
                endStr.erase(endStr.find_last_not_of(" \t") + 1);

                startTime = parseTime(startStr);
                endTime = parseTime(endStr);

                if (startTime < 0 || endTime < 0) {
                    std::cerr << "Failed to parse timing: " << line << std::endl;
                    state = NUMBER; // Reset to looking for a new subtitle
                }
                else {
                    state = TEXT;
                    subtitleText.clear(); // Clear text buffer for this subtitle
                }
            }
            else {
                std::cerr << "Unexpected line (expected timing): " << line << std::endl;
                state = NUMBER; // Reset to looking for a new subtitle
            }
            break;

        case TEXT:
            if (line.empty()) {
                // Empty line means end of this subtitle
                if (!subtitleText.empty()) {
                    // Add the subtitle to our collection
                    Subtitle subtitle = { startTime, endTime, subtitleText };
                    m_Subtitles.push_back(subtitle);

                    // Reset for the next subtitle
                    subtitleText.clear();
                    startTime = -1.0;
                    endTime = -1.0;
                }
                state = NUMBER;
            }
            else {
                // Append this line to the subtitle text
                if (!subtitleText.empty()) {
                    subtitleText += "\n";
                }
                subtitleText += line;
            }
            break;

        case BLANK:
            if (!line.empty()) {
                state = NUMBER;
                // Process this non-blank line
                try {
                    subtitleNumber = std::stoi(line);
                    state = TIMING;
                }
                catch (const std::exception&) {
                    std::cerr << "Unexpected line after blank: " << line << std::endl;
                }
            }
            break;
        }
    }

    // Don't forget the last subtitle if we're still processing it
    if (startTime >= 0 && endTime >= 0 && !subtitleText.empty()) {
        Subtitle subtitle = { startTime, endTime, subtitleText };
        m_Subtitles.push_back(subtitle);
    }

    file.close();

    std::cout << "Loaded " << m_Subtitles.size() << " subtitles from " << srtPath << std::endl;
}

double GameVideo::parseTime(const std::string& timeStr) {
    int hours = 0, minutes = 0, seconds = 0, milliseconds = 0;

    // Handle both comma and period as decimal separators in SRT files
    std::string normalizedStr = timeStr;
    size_t commaPos = normalizedStr.find(',');
    if (commaPos != std::string::npos) {
        normalizedStr[commaPos] = '.'; // Replace comma with period for consistent parsing
    }

    // Parse the time format "hh:mm:ss,SSS" or "hh:mm:ss.SSS"
    if (sscanf(normalizedStr.c_str(), "%d:%d:%d.%d", &hours, &minutes, &seconds, &milliseconds) == 4) {
        return hours * 3600.0 + minutes * 60.0 + seconds + milliseconds / 1000.0;
    }

    // If parsing fails, return a negative value to indicate error
    return -1.0;
}

float GameVideo::GetPosition()  {
    if (m_TotalDuration <= 0.0) {
        return 0.0f;  // Avoid division by zero
    }

    // Get the current playback time
    float currentTime = getSourceTime(source);

    // Calculate normalized position (0.0 to 1.0)
    float normalizedPosition = static_cast<float>(currentTime / m_TotalDuration);

    // Clamp between 0.0 and 1.0
    if (normalizedPosition < 0.0f) return 0.0f;
    if (normalizedPosition > 1.0f) return 1.0f;

    return normalizedPosition;
}


bool GameVideo::IsFinished() {
    // If the video isn't playing, we shouldn't consider it as finished
    if (!isPlaying) {
        return true;
    }

    // Get current playback position
    if (m_NoAudio) {
    
        int time = clock();
        int diff = time - StartClock;
        float ctime = ((float)diff) / (1000.0f);
        if (ctime >= m_TotalDuration) {
            return true;
        }
        else {
            return false;
        }
    
    }
    else {
        float currentTime = getSourceTime(source);

        // Check if we're at or past the end of the video
        // Using a small threshold to account for floating-point precision
        const double END_THRESHOLD = 0.1; // 100ms threshold

        return (currentTime >= (m_TotalDuration - END_THRESHOLD));

    }
    // Alternative approach: check if normalized position is very close to 1.0
    // return (GetNormalizedPosition() > 0.99f);
}

float GameVideo::GetLength() {

    return (float)m_TotalDuration;
};
