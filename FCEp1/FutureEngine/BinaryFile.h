#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <stdexcept>
#include <cstdint>
#include <filesystem>
#include <iostream>

class FFile {
private:
    std::fstream file;
    bool isReading;

public:
    // Constructor for writing mode
    FFile(const std::string& filename, bool reading = false) : isReading(reading) {
        if (reading) {
            file.open(filename, std::ios::in | std::ios::binary);
        }
        else {
            file.open(filename, std::ios::out | std::ios::binary);
        }

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
    }

    // Destructor
    ~FFile() {
        if (file.is_open()) {
            file.close();
        }
    }

    static bool copyFile(const std::string& sourcePath, const std::string& destinationPath) {
        try {
            // Check if source file exists
            if (!std::filesystem::exists(sourcePath)) {
                std::cerr << "Error: Source file does not exist: " << sourcePath << std::endl;
                return false;
            }

            // Check if source is a regular file
            if (!std::filesystem::is_regular_file(sourcePath)) {
                std::cerr << "Error: Source is not a regular file: " << sourcePath << std::endl;
                return false;
            }

            // Create destination directory if it doesn't exist
            std::filesystem::path destPath(destinationPath);
            if (destPath.has_parent_path()) {
                std::filesystem::create_directories(destPath.parent_path());
            }

            // Method 1: Using filesystem copy (C++17)
            std::filesystem::copy_file(
                sourcePath,
                destinationPath,
                std::filesystem::copy_options::overwrite_existing
            );

            return true;
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;

            // Try backup method for copying if the filesystem copy fails
            return copyFileUsingStreams(sourcePath, destinationPath);
        }
        catch (const std::exception& e) {
            std::cerr << "Error copying file: " << e.what() << std::endl;
            return false;
        }
    }

    // Backup method using file streams
    static bool copyFileUsingStreams(const std::string& sourcePath, const std::string& destinationPath) {
        try {
            // Open source file for reading in binary mode
            std::ifstream src(sourcePath, std::ios::binary);
            if (!src) {
                std::cerr << "Error: Cannot open source file: " << sourcePath << std::endl;
                return false;
            }

            // Open destination file for writing in binary mode
            std::ofstream dst(destinationPath, std::ios::binary);
            if (!dst) {
                std::cerr << "Error: Cannot open destination file: " << destinationPath << std::endl;
                return false;
            }

            // Copy file content
            dst << src.rdbuf();

            // Check if both streams are still good
            if (!src || !dst) {
                std::cerr << "Error: File streaming error occurred during copy" << std::endl;
                return false;
            }

            // Close files
            src.close();
            dst.close();

            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error in stream copy: " << e.what() << std::endl;
            return false;
        }
    }

    static bool createDirectory(const std::string& path) {
        try {
            std::filesystem::create_directories(path);
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error creating directory: " << e.what() << std::endl;
            return false;
        }
    }
    static std::string getFileExtension(const std::string& path) {
        // C++17 filesystem approach
        std::filesystem::path filePath(path);
        std::string extension = filePath.extension().string();

        return extension;
    }

    static bool fileExists(const std::string& filename) {
        std::ifstream file(filename);
        return file.good();
    }

    // ---- Writing functions for basic types ----

    void writeByte(uint8_t value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(uint8_t));
    }

    void writeBool(bool value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(bool));
    }

    void writeChar(char value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(char));
    }

    void writeUChar(unsigned char value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(unsigned char));
    }

    void writeShort(int16_t value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(int16_t));
    }

    void writeUShort(uint16_t value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(uint16_t));
    }

    void writeInt(int32_t value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(int32_t));
    }

    void writeUInt(uint32_t value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(uint32_t));
    }

    void writeLong(int64_t value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(int64_t));
    }

    void writeULong(uint64_t value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(uint64_t));
    }

    void writeFloat(float value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(float));
    }

    void writeDouble(double value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(double));
    }

    // ---- Writing functions for GLM types ----

    void writeVec2(const glm::vec2& value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(glm::vec2));
    }

    void writeVec3(const glm::vec3& value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(glm::vec3));
    }

    void writeVec4(const glm::vec4& value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(glm::vec4));
    }

    void writeIVec2(const glm::ivec2& value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(glm::ivec2));
    }

    void writeIVec3(const glm::ivec3& value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(glm::ivec3));
    }

    void writeIVec4(const glm::ivec4& value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(glm::ivec4));
    }

    void writeMat2(const glm::mat2& value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(glm::mat2));
    }

    void writeMat3(const glm::mat3& value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(glm::mat3));
    }

    void writeMat4(const glm::mat4& value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");
        file.write(reinterpret_cast<const char*>(&value), sizeof(glm::mat4));
    }


    // ---- Writing functions for compound types ----

    void writeString(const std::string& value) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");

        // Write the string length first
        uint32_t length = static_cast<uint32_t>(value.length());
        writeUInt(length);

        // Then write the string content
        if (length > 0) {
            file.write(value.c_str(), length);
        }
    }

    template<typename T>
    void writeVector(const std::vector<T>& vec) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");

        // Write the vector size
        uint32_t size = static_cast<uint32_t>(vec.size());
        writeUInt(size);

        // Write each element
        if (size > 0) {
            file.write(reinterpret_cast<const char*>(vec.data()), sizeof(T) * size);
        }
    }

    // Special case for vector<string>
    void writeStringVector(const std::vector<std::string>& vec) {
        if (isReading) throw std::runtime_error("Cannot write to file opened for reading");

        // Write the vector size
        uint32_t size = static_cast<uint32_t>(vec.size());
        writeUInt(size);

        // Write each string
        for (const auto& str : vec) {
            writeString(str);
        }
    }

    // ---- Reading functions for basic types ----

    uint8_t readByte() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        uint8_t value;
        file.read(reinterpret_cast<char*>(&value), sizeof(uint8_t));

        if (file.fail()) {
            throw std::runtime_error("Failed to read byte from file");
        }

        return value;
    }

    bool readBool() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        bool value;
        file.read(reinterpret_cast<char*>(&value), sizeof(bool));

        if (file.fail()) {
            throw std::runtime_error("Failed to read bool from file");
        }

        return value;
    }

    char readChar() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        char value;
        file.read(reinterpret_cast<char*>(&value), sizeof(char));

        if (file.fail()) {
            throw std::runtime_error("Failed to read char from file");
        }

        return value;
    }

    unsigned char readUChar() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        unsigned char value;
        file.read(reinterpret_cast<char*>(&value), sizeof(unsigned char));

        if (file.fail()) {
            throw std::runtime_error("Failed to read unsigned char from file");
        }

        return value;
    }

    int16_t readShort() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        int16_t value;
        file.read(reinterpret_cast<char*>(&value), sizeof(int16_t));

        if (file.fail()) {
            throw std::runtime_error("Failed to read short from file");
        }

        return value;
    }

    uint16_t readUShort() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        uint16_t value;
        file.read(reinterpret_cast<char*>(&value), sizeof(uint16_t));

        if (file.fail()) {
            throw std::runtime_error("Failed to read unsigned short from file");
        }

        return value;
    }

    int32_t readInt() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        int32_t value;
        file.read(reinterpret_cast<char*>(&value), sizeof(int32_t));

        if (file.fail()) {
            throw std::runtime_error("Failed to read int from file");
        }

        return value;
    }

    uint32_t readUInt() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        uint32_t value;
        file.read(reinterpret_cast<char*>(&value), sizeof(uint32_t));

        if (file.fail()) {
            throw std::runtime_error("Failed to read unsigned int from file");
        }

        return value;
    }

    int64_t readLong() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        int64_t value;
        file.read(reinterpret_cast<char*>(&value), sizeof(int64_t));

        if (file.fail()) {
            throw std::runtime_error("Failed to read long from file");
        }

        return value;
    }

    uint64_t readULong() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        uint64_t value;
        file.read(reinterpret_cast<char*>(&value), sizeof(uint64_t));

        if (file.fail()) {
            throw std::runtime_error("Failed to read unsigned long from file");
        }

        return value;
    }

    float readFloat() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        float value;
        file.read(reinterpret_cast<char*>(&value), sizeof(float));

        if (file.fail()) {
            throw std::runtime_error("Failed to read float from file");
        }

        return value;
    }

    double readDouble() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        double value;
        file.read(reinterpret_cast<char*>(&value), sizeof(double));

        if (file.fail()) {
            throw std::runtime_error("Failed to read double from file");
        }

        return value;
    }

    // ---- Reading functions for GLM types ----

    glm::vec2 readVec2() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        glm::vec2 value;
        file.read(reinterpret_cast<char*>(&value), sizeof(glm::vec2));

        if (file.fail()) {
            throw std::runtime_error("Failed to read vec2 from file");
        }

        return value;
    }

    glm::vec3 readVec3() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        glm::vec3 value;
        file.read(reinterpret_cast<char*>(&value), sizeof(glm::vec3));

        if (file.fail()) {
            throw std::runtime_error("Failed to read vec3 from file");
        }

        return value;
    }

    glm::vec4 readVec4() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        glm::vec4 value;
        file.read(reinterpret_cast<char*>(&value), sizeof(glm::vec4));

        if (file.fail()) {
            throw std::runtime_error("Failed to read vec4 from file");
        }

        return value;
    }

    glm::ivec2 readIVec2() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        glm::ivec2 value;
        file.read(reinterpret_cast<char*>(&value), sizeof(glm::ivec2));

        if (file.fail()) {
            throw std::runtime_error("Failed to read ivec2 from file");
        }

        return value;
    }

    glm::ivec3 readIVec3() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        glm::ivec3 value;
        file.read(reinterpret_cast<char*>(&value), sizeof(glm::ivec3));

        if (file.fail()) {
            throw std::runtime_error("Failed to read ivec3 from file");
        }

        return value;
    }

    glm::ivec4 readIVec4() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        glm::ivec4 value;
        file.read(reinterpret_cast<char*>(&value), sizeof(glm::ivec4));

        if (file.fail()) {
            throw std::runtime_error("Failed to read ivec4 from file");
        }

        return value;
    }

    glm::mat2 readMat2() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        glm::mat2 value;
        file.read(reinterpret_cast<char*>(&value), sizeof(glm::mat2));

        if (file.fail()) {
            throw std::runtime_error("Failed to read mat2 from file");
        }

        return value;
    }

    glm::mat3 readMat3() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        glm::mat3 value;
        file.read(reinterpret_cast<char*>(&value), sizeof(glm::mat3));

        if (file.fail()) {
            throw std::runtime_error("Failed to read mat3 from file");
        }

        return value;
    }

    glm::mat4 readMat4() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        glm::mat4 value;
        file.read(reinterpret_cast<char*>(&value), sizeof(glm::mat4));

        if (file.fail()) {
            throw std::runtime_error("Failed to read mat4 from file");
        }

        return value;
    }

  
    // ---- Reading functions for compound types ----

    std::string readString() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        // Read the string length first
        uint32_t length = readUInt();

        // Then read the string content
        std::string value;
        if (length > 0) {
            value.resize(length);
            file.read(&value[0], length);

            if (file.fail()) {
                throw std::runtime_error("Failed to read string from file");
            }
        }

        return value;
    }

    template<typename T>
    std::vector<T> readVector() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        // Read the vector size
        uint32_t size = readUInt();

        // Read the elements
        std::vector<T> vec(size);
        if (size > 0) {
            file.read(reinterpret_cast<char*>(vec.data()), sizeof(T) * size);

            if (file.fail()) {
                throw std::runtime_error("Failed to read vector from file");
            }
        }

        return vec;
    }

    // Special case for vector<string>
    std::vector<std::string> readStringVector() {
        if (!isReading) throw std::runtime_error("Cannot read from file opened for writing");

        // Read the vector size
        uint32_t size = readUInt();

        // Read each string
        std::vector<std::string> vec;
        vec.reserve(size);
        for (uint32_t i = 0; i < size; ++i) {
            vec.push_back(readString());
        }

        return vec;
    }

    // ---- Utility functions ----

    bool isOpen() const {
        return file.is_open();
    }

    bool eof() const {
        return file.eof();
    }

    void close() {
        if (file.is_open()) {
            file.flush();
            file.close();
        }
    }

    // Get current position in file
    std::streampos tell() {
        if (isReading) {
            return file.tellg();
        }
        else {
            return file.tellp();
        }
    }

    // Seek to position in file
    void seek(std::streampos pos) {
        if (isReading) {
            file.seekg(pos);
        }
        else {
            file.seekp(pos);
        }
    }

    // Skip ahead by offset bytes
    void skip(std::streamoff offset) {
        if (isReading) {
            file.seekg(offset, std::ios::cur);
        }
        else {
            file.seekp(offset, std::ios::cur);
        }
    }
};