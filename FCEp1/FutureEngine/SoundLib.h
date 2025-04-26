// SoundLibrary.h - Main header file
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <AL/al.h>
#include <AL/alc.h>

// Forward declarations
class Sound;
class Channel;

// Error handling
enum class SoundError {
    NONE,
    FILE_NOT_FOUND,
    INVALID_FILE_FORMAT,
    AL_ERROR,
    DEVICE_ERROR,
    UNSUPPORTED_FORMAT  // For MP3 files that we don't support yet
};

// Sound type enum
enum class SoundType {
    WAV,
    MP3,      // Keeping this for future implementation
    UNKNOWN
};

// Playback mode
enum class PlayMode {
    MEMORY,    // Load entire sound into memory
    STREAMING  // Stream from disk
};

// WAV file header structure
#pragma pack(push, 1)
struct WAVHeader {
    // RIFF Header
    char riffHeader[4];    // Contains "RIFF"
    uint32_t wavSize;      // Size of the wav portion of the file
    char waveHeader[4];    // Contains "WAVE"

    // Format Header
    char fmtHeader[4];     // Contains "fmt "
    uint32_t fmtChunkSize; // Size of the format section
    uint16_t audioFormat;  // Audio format type (1 = PCM)
    uint16_t numChannels;  // Number of channels
    uint32_t sampleRate;   // Sample rate
    uint32_t byteRate;     // Bytes per second
    uint16_t blockAlign;   // Bytes for one sample including all channels
    uint16_t bitsPerSample;// Bits per sample

    // Data Header
    char dataHeader[4];    // Contains "data"
    uint32_t dataSize;     // Number of bytes in data
};
#pragma pack(pop)

// Sound class
class Sound {
public:
    Sound(class SoundLIB* lib, const std::string& filename, PlayMode mode);
    ~Sound();

    // Status
    bool isLoaded() const;
    PlayMode getPlayMode() const;
    SoundType getType() const;
    float getDuration() const;
    const std::string& getFilename() const;

    // Internal buffer management
    ALuint getBuffer() const;

    // Streaming functions
    bool updateStream(ALuint buffer);
    void rewind();

private:
    // Parent library
    class SoundLIB* mLib;

    // Sound properties
    std::string mFilename;
    SoundType mType;
    PlayMode mMode;
    float mDuration;
    bool mLoaded;

    // OpenAL buffer (for memory mode)
    ALuint mBuffer;

    // File data for streaming
    FILE* mFile;
    std::vector<char> mTempBuffer;
    size_t mFileSize;
    size_t mCurPosition;

    // Format info
    ALenum mFormat;
    ALsizei mSampleRate;

    // Helper functions
    bool loadWAV();
    // MP3 functions would be added here in the future
    // bool loadMP3();

    bool streamWAV(ALuint buffer);
    // bool streamMP3(ALuint buffer);

    void closeFile();

    // Friends
    friend class SoundLIB;
    friend class Channel;
};

// Channel class
class Channel {
public:
    Channel(class SoundLIB* lib, std::shared_ptr<Sound> sound);
    ~Channel();

    // Playback control
    bool play(float volume = 1.0f, float pitch = 1.0f, bool loop = false);
    void stop();
    void pause();
    void resume();

    // Status
    bool isPlaying() const;
    bool isPaused() const;
    float getPlaybackPosition() const;

    // Properties
    void setVolume(float volume);
    float getVolume() const;

    void setPitch(float pitch);
    float getPitch() const;

    void setPosition(float x, float y, float z);
    void getPosition(float& x, float& y, float& z) const;

    void setVelocity(float x, float y, float z);
    void getVelocity(float& x, float& y, float& z) const;

    void setLooping(bool loop);
    bool isLooping() const;

    // Update function for streaming
    void update();

    // Get the sound
    std::shared_ptr<Sound> getSound() const;

private:
    // Parent library and sound
    class SoundLIB* mLib;
    std::shared_ptr<Sound> mSound;

    // OpenAL source
    ALuint mSource;

    // Status
    bool mPlaying;
    bool mPaused;

    // Handle to remove from library's channel list
    void invalidate();

    // Friends
    friend class SoundLIB;
};

// Main SoundLIB class
class SoundLIB {
public:
    SoundLIB();
    ~SoundLIB();

    // Initialize and shutdown
    bool initialize();
    void shutdown();

    // Sound loading functions
    std::shared_ptr<Sound> loadSound(const std::string& filename, PlayMode mode = PlayMode::MEMORY);

    // Play sounds and get channel for control
    std::shared_ptr<Channel> playSound(std::shared_ptr<Sound> sound, float volume = 1.0f, float pitch = 1.0f, bool loop = false);

    // Stop all sounds
    void stopAllSounds();

    // Update function (call in main loop for streaming)
    void update();

    // Get last error
    SoundError getLastError() const;
    std::string getErrorString() const;

private:
    // OpenAL device and context
    ALCdevice* mDevice;
    ALCcontext* mContext;

    // Sound storage
    std::unordered_map<std::string, std::shared_ptr<Sound>> mSounds;

    // Channel management
    std::vector<std::shared_ptr<Channel>> mChannels;

    // Error tracking
    SoundError mLastError;

    // Helper functions
    SoundType getSoundTypeFromFile(const std::string& filename);
    std::string getFileExtension(const std::string& filename);

    // Friends
    friend class Sound;
    friend class Channel;
};