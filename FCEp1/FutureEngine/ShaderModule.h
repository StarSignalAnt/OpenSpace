#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ShaderModule
{
public:
    // Constructor that takes the paths to the vertex and fragment shaders
    ShaderModule(const std::string& vertexPath, const std::string& fragPath);

    // Destructor to clean up the shader program
    ~ShaderModule();

    // Function to bind the shader program
    void Bind() const;

    // Function to unbind the shader program
    void Unbind() const;

    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
	void SetVec2(const std::string& name, const glm::vec2& value) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetMat4(const std::string& name, const glm::mat4& value) const;


private:
    GLuint programID; // Shader program ID
    GLuint compileShader(const std::string& path, GLenum shaderType) const; // Helper function to compile a shader from file
    std::string loadShaderSource(const std::string& path) const;
    GLint getUniformLocation(const std::string& name) const;
};

