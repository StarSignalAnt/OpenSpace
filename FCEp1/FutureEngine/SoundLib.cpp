#include "SoundLib.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// SoundLIB implementation
// Constructor
SoundLIB::SoundLIB() : mDevice(nullptr), mContext(nullptr), mLastError(SoundError::NONE) {
}

// Destructor
SoundLIB::~SoundLIB() {
    shutdown();
}

// Initialize OpenAL
bool SoundLIB::initialize() {
    // Open the default device
    mDevice = alcOpenDevice(nullptr);
    if (!mDevice) {
        mLastError = SoundError::DEVICE_ERROR;
        return false;
    }

    // Create context
    mContext = alcCreateContext(mDevice, nullptr);
    if (!mContext) {
        alcCloseDevice(mDevice);
        mDevice = nullptr;
        mLastError = SoundError::DEVICE_ERROR;
        return false;
    }

    // Make the context current
    if (!alcMakeContextCurrent(mContext)) {
        alcDestroyContext(mContext);
        alcCloseDevice(mDevice);
        mContext = nullptr;
        mDevice = nullptr;
        mLastError = SoundError::DEVICE_ERROR;
        return false;
    }

    return true;
}

// Shutdown OpenAL
void SoundLIB::shutdown() {
    // Stop all channels
    stopAllSounds();

    // Clear sounds
    mSounds.clear();
    mChannels.clear();

    // Clean up OpenAL
    if (mContext) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(mContext);
        mContext = nullptr;
    }

    if (mDevice) {
        alcCloseDevice(mDevice);
        mDevice = nullptr;
    }
}

// Load a sound from file
std::shared_ptr<Sound> SoundLIB::loadSound(const std::string& filename, PlayMode mode) {
    // Check if sound is already loaded
    auto it = mSounds.find(filename);
    if (it != mSounds.end()) {
        return it->second;
    }

    // Check file type
    SoundType type = getSoundTypeFromFile(filename);
    if (type == SoundType::MP3) {
        // MP3 support is not yet implemented
        mLastError = SoundError::UNSUPPORTED_FORMAT;
        return nullptr;
    }

    // Create new sound
    auto sound = std::make_shared<Sound>(this, filename, mode);
    if (!sound->isLoaded()) {
        // Error loading sound
        return nullptr;
    }

    // Store in the map
    mSounds[filename] = sound;
    return sound;
}

// Play a sound and get a channel for control
std::shared_ptr<Channel> SoundLIB::playSound(std::shared_ptr<Sound> sound, float volume, float pitch, bool loop) {
    if (!sound || !sound->isLoaded()) {
        mLastError = SoundError::INVALID_FILE_FORMAT;
        return nullptr;
    }

    // Create a new channel
    auto channel = std::make_shared<Channel>(this, sound);

    // Play the sound
    if (!channel->play(volume, pitch, loop)) {
        return nullptr;
    }

    // Add to channel list
    mChannels.push_back(channel);

    return channel;
}

// Stop all sounds
void SoundLIB::stopAllSounds() {
    for (auto& channel : mChannels) {
        channel->stop();
    }
    mChannels.clear();
}

// Update function for streaming
void SoundLIB::update() {
    // Update all channels
    auto it = mChannels.begin();
    while (it != mChannels.end()) {
        auto channel = *it;

        if (channel->isPlaying()) {
            channel->update();
            ++it;
        }
        else {
            // Remove channels that are no longer playing
            it = mChannels.erase(it);
        }
    }
}

// Get last error
SoundError SoundLIB::getLastError() const {
    return mLastError;
}

// Get error string
std::string SoundLIB::getErrorString() const {
    switch (mLastError) {
    case SoundError::NONE:
        return "No error";
    case SoundError::FILE_NOT_FOUND:
        return "File not found";
    case SoundError::INVALID_FILE_FORMAT:
        return "Invalid file format";
    case SoundError::AL_ERROR:
        return "OpenAL error: " + std::string(alGetString(alGetError()));
    case SoundError::DEVICE_ERROR:
        return "Audio device error";
    case SoundError::UNSUPPORTED_FORMAT:
        return "Unsupported audio format (MP3 support not yet implemented)";
    default:
        return "Unknown error";
    }
}

// Determine sound type from file extension
SoundType SoundLIB::getSoundTypeFromFile(const std::string& filename) {
    std::string ext = getFileExtension(filename);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "wav") return SoundType::WAV;
    if (ext == "mp3") return SoundType::MP3;

    return SoundType::UNKNOWN;
}

// Get file extension
std::string SoundLIB::getFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of(".");
    if (pos != std::string::npos) {
        return filename.substr(pos + 1);
    }
    return "";
}

// Sound class implementation
// Sound constructor
Sound::Sound(SoundLIB* lib, const std::string& filename, PlayMode mode)
    : mLib(lib), mFilename(filename), mMode(mode), mDuration(0.0f), mLoaded(false),
    mBuffer(0), mFile(nullptr), mFileSize(0), mCurPosition(0),
    mFormat(0), mSampleRate(0) {

    // Determine sound type
    mType = mLib->getSoundTypeFromFile(filename);

    // Check for MP3 (not supported yet)
    if (mType == SoundType::MP3) {
        mLib->mLastError = SoundError::UNSUPPORTED_FORMAT;
        return;
    }

    // Create buffer for memory mode
    if (mMode == PlayMode::MEMORY) {
        alGenBuffers(1, &mBuffer);
        if (alGetError() != AL_NO_ERROR) {
            mLib->mLastError = SoundError::AL_ERROR;
            return;
        }
    }
    else if (mMode == PlayMode::STREAMING) {
        // Create streaming buffers (we'll use 3 for a triple-buffer approach)
        ALuint buffers[3];
        alGenBuffers(3, buffers);
        if (alGetError() != AL_NO_ERROR) {
            mLib->mLastError = SoundError::AL_ERROR;
            return;
        }

        // Store the buffer IDs in our temp buffer for later use
        mTempBuffer.resize(sizeof(ALuint) * 3);
        memcpy(mTempBuffer.data(), buffers, sizeof(ALuint) * 3);
    }

    // Load or prepare for streaming
    switch (mType) {
    case SoundType::WAV:
        if (mMode == PlayMode::MEMORY) {
            mLoaded = loadWAV();
        }
        else {
            // Open file for streaming
            mFile = fopen(filename.c_str(), "rb");
            if (!mFile) {
                mLib->mLastError = SoundError::FILE_NOT_FOUND;
                return;
            }

            // Read header to get format
            WAVHeader header;
            if (fread(&header, sizeof(WAVHeader), 1, mFile) != 1) {
                fclose(mFile);
                mFile = nullptr;
                mLib->mLastError = SoundError::INVALID_FILE_FORMAT;
                return;
            }

            // Set format
            if (header.numChannels == 1) {
                if (header.bitsPerSample == 8) mFormat = AL_FORMAT_MONO8;
                else if (header.bitsPerSample == 16) mFormat = AL_FORMAT_MONO16;
            }
            else if (header.numChannels == 2) {
                if (header.bitsPerSample == 8) mFormat = AL_FORMAT_STEREO8;
                else if (header.bitsPerSample == 16) mFormat = AL_FORMAT_STEREO16;
            }

            mSampleRate = header.sampleRate;
            mFileSize = header.dataSize;
            mCurPosition = 0;

            // Allocate temp buffer for streaming data (16KB)
            mTempBuffer.resize(16384 + sizeof(ALuint) * 3);

            // Calculate duration
            if (header.byteRate > 0) {
                mDuration = static_cast<float>(header.dataSize) / header.byteRate;
            }

            // Rewind to data position
            fseek(mFile, sizeof(WAVHeader), SEEK_SET);
            mLoaded = true;
        }
        break;

    case SoundType::MP3:
        // MP3 support would be added here in the future
        mLib->mLastError = SoundError::UNSUPPORTED_FORMAT;
        break;

    default:
        mLib->mLastError = SoundError::INVALID_FILE_FORMAT;
        break;
    }
}

// Sound destructor
Sound::~Sound() {
    // Clean up file
    closeFile();

    // Clean up OpenAL buffers
    if (mBuffer) {
        alDeleteBuffers(1, &mBuffer);
    }

    // Delete streaming buffers if we have them
    if (mMode == PlayMode::STREAMING && !mTempBuffer.empty() && mTempBuffer.size() >= sizeof(ALuint) * 3) {
        ALuint buffers[3];
        memcpy(buffers, mTempBuffer.data(), sizeof(ALuint) * 3);
        alDeleteBuffers(3, buffers);
    }
}

// Status
bool Sound::isLoaded() const {
    return mLoaded;
}

PlayMode Sound::getPlayMode() const {
    return mMode;
}

SoundType Sound::getType() const {
    return mType;
}

float Sound::getDuration() const {
    return mDuration;
}

const std::string& Sound::getFilename() const {
    return mFilename;
}

// Get OpenAL buffer
ALuint Sound::getBuffer() const {
    return mBuffer;
}

// Update stream buffers
bool Sound::updateStream(ALuint buffer) {
    if (mType == SoundType::WAV) {
        return streamWAV(buffer);
    }
    // MP3 streaming would be added here in the future
    return false;
}

// Rewind stream
void Sound::rewind() {
    if (mFile) {
        fseek(mFile, sizeof(WAVHeader), SEEK_SET);
        mCurPosition = 0;
    }
}

// Load WAV file into memory
bool Sound::loadWAV() {
    // Open the file
    FILE* file = fopen(mFilename.c_str(), "rb");
    if (!file) {
        mLib->mLastError = SoundError::FILE_NOT_FOUND;
        return false;
    }

    // Read the header
    WAVHeader header;
    if (fread(&header, sizeof(WAVHeader), 1, file) != 1) {
        fclose(file);
        mLib->mLastError = SoundError::INVALID_FILE_FORMAT;
        return false;
    }

    // Check RIFF header
    if (memcmp(header.riffHeader, "RIFF", 4) != 0 ||
        memcmp(header.waveHeader, "WAVE", 4) != 0 ||
        memcmp(header.fmtHeader, "fmt ", 4) != 0 ||
        memcmp(header.dataHeader, "data", 4) != 0) {
        fclose(file);
        mLib->mLastError = SoundError::INVALID_FILE_FORMAT;
        return false;
    }

    // Read the audio data
    std::vector<char> data(header.dataSize);
    if (fread(data.data(), header.dataSize, 1, file) != 1) {
        fclose(file);
        mLib->mLastError = SoundError::INVALID_FILE_FORMAT;
        return false;
    }

    // Close the file
    fclose(file);

    // Determine format
    ALenum format;
    if (header.numChannels == 1) {
        if (header.bitsPerSample == 8) format = AL_FORMAT_MONO8;
        else if (header.bitsPerSample == 16) format = AL_FORMAT_MONO16;
        else {
            mLib->mLastError = SoundError::INVALID_FILE_FORMAT;
            return false;
        }
    }
    else if (header.numChannels == 2) {
        if (header.bitsPerSample == 8) format = AL_FORMAT_STEREO8;
        else if (header.bitsPerSample == 16) format = AL_FORMAT_STEREO16;
        else {
            mLib->mLastError = SoundError::INVALID_FILE_FORMAT;
            return false;
        }
    }
    else {
        mLib->mLastError = SoundError::INVALID_FILE_FORMAT;
        return false;
    }

    // Upload the buffer
    alBufferData(mBuffer, format, data.data(), static_cast<ALsizei>(data.size()), header.sampleRate);
    if (alGetError() != AL_NO_ERROR) {
        mLib->mLastError = SoundError::AL_ERROR;
        return false;
    }

    // Calculate duration
    mDuration = static_cast<float>(header.dataSize) / header.byteRate;
    mFormat = format;
    mSampleRate = header.sampleRate;

    return true;
}

// Stream chunk of WAV file
bool Sound::streamWAV(ALuint buffer) {
    if (!mFile) {
        return false;
    }

    // Read a chunk of data
    size_t dataOffset = sizeof(ALuint) * 3;  // Skip the buffer IDs at the start
    size_t bytesToRead = mTempBuffer.size() - dataOffset;
    size_t bytesRead = fread(mTempBuffer.data() + dataOffset, 1, bytesToRead, mFile);

    if (bytesRead > 0) {
        // Buffer the data
        alBufferData(buffer, mFormat, mTempBuffer.data() + dataOffset, static_cast<ALsizei>(bytesRead), mSampleRate);
        if (alGetError() != AL_NO_ERROR) {
            return false;
        }

        mCurPosition += bytesRead;
        return true;
    }

    // End of file or error
    return false;
}

// Close file handle
void Sound::closeFile() {
    if (mFile) {
        fclose(mFile);
        mFile = nullptr;
    }
}

// Channel class implementation
// Channel constructor
Channel::Channel(SoundLIB* lib, std::shared_ptr<Sound> sound)
    : mLib(lib), mSound(sound), mSource(0), mPlaying(false), mPaused(false) {

    // Generate source
    alGenSources(1, &mSource);
    if (alGetError() != AL_NO_ERROR) {
        mLib->mLastError = SoundError::AL_ERROR;
        return;
    }

    // Set source properties
    alSource3f(mSource, AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(mSource, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alSource3f(mSource, AL_DIRECTION, 0.0f, 0.0f, 0.0f);
    alSourcef(mSource, AL_ROLLOFF_FACTOR, 0.0f);
    alSourcei(mSource, AL_SOURCE_RELATIVE, AL_TRUE);
}

// Channel destructor
Channel::~Channel() {
    // Stop playback
    stop();

    // Delete source
    if (mSource) {
        alDeleteSources(1, &mSource);
    }
}

// Play the sound
bool Channel::play(float volume, float pitch, bool loop) {
    if (!mSound || !mSource) {
        return false;
    }

    // Stop if already playing
    if (mPlaying) {
        alSourceStop(mSource);
    }

    // Set properties
    setVolume(volume);
    setPitch(pitch);
    setLooping(loop);

    // Handle different playback modes
    if (mSound->getPlayMode() == PlayMode::MEMORY) {
        // Attach buffer
        alSourcei(mSource, AL_BUFFER, mSound->getBuffer());
        if (alGetError() != AL_NO_ERROR) {
            mLib->mLastError = SoundError::AL_ERROR;
            return false;
        }

        // Start playback
        alSourcePlay(mSource);
        if (alGetError() != AL_NO_ERROR) {
            mLib->mLastError = SoundError::AL_ERROR;
            return false;
        }
    }
    else if (mSound->getPlayMode() == PlayMode::STREAMING) {
        // For streaming, we'll queue initial buffers
        mSound->rewind();

        // Get streaming buffers (stored in the first part of mTempBuffer)
        ALuint buffers[3];
        memcpy(buffers, mSound->mTempBuffer.data(), sizeof(ALuint) * 3);

        // Queue initial buffers
        for (int i = 0; i < 3; i++) {
            if (mSound->updateStream(buffers[i])) {
                alSourceQueueBuffers(mSource, 1, &buffers[i]);
                if (alGetError() != AL_NO_ERROR) {
                    mLib->mLastError = SoundError::AL_ERROR;
                    return false;
                }
            }
        }

        // Start playback
        alSourcePlay(mSource);
        if (alGetError() != AL_NO_ERROR) {
            mLib->mLastError = SoundError::AL_ERROR;
            return false;
        }
    }

    mPlaying = true;
    mPaused = false;
    return true;
}

// Stop playback
void Channel::stop() {
    if (mSource) {
        alSourceStop(mSource);

        // Unqueue buffers for streaming sounds
        if (mSound && mSound->getPlayMode() == PlayMode::STREAMING) {
            // Get number of queued buffers
            ALint queued;
            alGetSourcei(mSource, AL_BUFFERS_QUEUED, &queued);

            // Unqueue them
            ALuint buffer;
            for (ALint i = 0; i < queued; i++) {
                alSourceUnqueueBuffers(mSource, 1, &buffer);
            }
        }
    }

    mPlaying = false;
    mPaused = false;
}

// Pause playback
void Channel::pause() {
    if (mPlaying && !mPaused && mSource) {
        alSourcePause(mSource);
        mPaused = true;
    }
}

// Resume playback
void Channel::resume() {
    if (mPlaying && mPaused && mSource) {
        alSourcePlay(mSource);
        mPaused = false;
    }
}

// Check if playing
bool Channel::isPlaying() const {
    if (!mSource) return false;

    if (mPlaying) {
        // Check actual state
        ALint state;
        alGetSourcei(mSource, AL_SOURCE_STATE, &state);
        return state == AL_PLAYING || state == AL_PAUSED;
    }

    return false;
}

// Check if paused
bool Channel::isPaused() const {
    return mPaused;
}

// Get playback position in seconds
float Channel::getPlaybackPosition() const {
    if (!mSource) return 0.0f;

    float pos;
    alGetSourcef(mSource, AL_SEC_OFFSET, &pos);
    return pos;
}

// Set volume (0.0 to 1.0)
void Channel::setVolume(float volume) {
    if (mSource) {
        alSourcef(mSource, AL_GAIN, volume);
    }
}

// Get volume
float Channel::getVolume() const {
    if (!mSource) return 0.0f;

    float volume;
    alGetSourcef(mSource, AL_GAIN, &volume);
    return volume;
}

// Set pitch (1.0 = normal)
void Channel::setPitch(float pitch) {
    if (mSource) {
        alSourcef(mSource, AL_PITCH, pitch);
    }
}

// Get pitch
float Channel::getPitch() const {
    if (!mSource) return 1.0f;

    float pitch;
    alGetSourcef(mSource, AL_PITCH, &pitch);
    return pitch;
}

// Set 3D position
void Channel::setPosition(float x, float y, float z) {
    if (mSource) {
        alSource3f(mSource, AL_POSITION, x, y, z);
    }
}

// Get 3D position
void Channel::getPosition(float& x, float& y, float& z) const {
    if (mSource) {
        alGetSource3f(mSource, AL_POSITION, &x, &y, &z);
    }
    else {
        x = y = z = 0.0f;
    }
}

// Set 3D velocity
void Channel::setVelocity(float x, float y, float z) {
    if (mSource) {
        alSource3f(mSource, AL_VELOCITY, x, y, z);
    }
}

// Get 3D velocity
void Channel::getVelocity(float& x, float& y, float& z) const {
    if (mSource) {
        alGetSource3f(mSource, AL_VELOCITY, &x, &y, &z);
    }
    else {
        x = y = z = 0.0f;
    }
}

// Set looping
void Channel::setLooping(bool loop) {
    if (mSource) {
        alSourcei(mSource, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    }
}

// Check if looping
bool Channel::isLooping() const {
    if (!mSource) return false;

    ALint loop;
    alGetSourcei(mSource, AL_LOOPING, &loop);
    return loop == AL_TRUE;
}

// Update streaming
void Channel::update() {
    // Only needed for streaming sounds
    if (!mSound || mSound->getPlayMode() != PlayMode::STREAMING || !mSource || !mPlaying || mPaused) {
        return;
    }

    // Check how many buffers have been processed
    ALint processed;
    alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);

    // Get buffer IDs
    ALuint buffers[3];
    memcpy(buffers, mSound->mTempBuffer.data(), sizeof(ALuint) * 3);

    // Unqueue and refill processed buffers
    ALuint buffer;
    while (processed > 0) {
        // Unqueue the buffer
        alSourceUnqueueBuffers(mSource, 1, &buffer);

        // Refill the buffer
        if (mSound->updateStream(buffer)) {
            // Queue it again
            alSourceQueueBuffers(mSource, 1, &buffer);
        }

        processed--;
    }

    // Check if we need to restart playback (buffer underrun)
    ALint state;
    alGetSourcei(mSource, AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING && state != AL_PAUSED) {
        // Get number of queued buffers
        ALint queued;
        alGetSourcei(mSource, AL_BUFFERS_QUEUED, &queued);

        if (queued > 0) {
            // Restart playback
            alSourcePlay(mSource);
        }
        else {
            // No more data, we're done
            mPlaying = false;
        }
    }
}

// Get the sound
std::shared_ptr<Sound> Channel::getSound() const {
    return mSound;
}

// Mark as invalid (for removing from sound lib)
void Channel::invalidate() {
    mPlaying = false;
}