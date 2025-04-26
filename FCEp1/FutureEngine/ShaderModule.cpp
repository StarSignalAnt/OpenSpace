#include "ShaderModule.h"

#include <iostream>
#include <fstream>
#include <sstream>

ShaderModule::ShaderModule(const std::string& vertexPath, const std::string& fragPath) {
    // Load and compile the vertex and fragment shaders
  //  / Load and compile the vertex and fragment shaders
        GLuint vertexShader = compileShader(vertexPath, GL_VERTEX_SHADER);
    GLuint fragShader = compileShader(fragPath, GL_FRAGMENT_SHADER);

    std::cout << "Vertex shader compiled successfully: " << vertexPath << std::endl;
    std::cout << "Fragment shader compiled successfully: " << fragPath << std::endl;

    // Create a shader program and link shaders
    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragShader);
    glLinkProgram(programID);

    // Check for linking errors
    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    else {
        std::cout << "Shader program linked successfully!" << std::endl;
    }

    // Clean up shaders after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);
}

ShaderModule::~ShaderModule() {
    // Delete the shader program when the object is destroyed
    glDeleteProgram(programID);
}

void ShaderModule::Bind() const {
    glUseProgram(programID); // Activate the shader program
}

void ShaderModule::Unbind() const {
    glUseProgram(0); // Deactivate the shader program
}

GLuint ShaderModule::compileShader(const std::string& path, GLenum shaderType) const {
    // Load shader source from file
    std::string shaderSource = loadShaderSource(path);
    const char* source = shaderSource.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

std::string ShaderModule::loadShaderSource(const std::string& path) const {
    std::ifstream shaderFile(path);

    // Ensure ifstream objects can throw exceptions
    if (!shaderFile.is_open()) {
        std::cerr << "ERROR::SHADER::FILE_NOT_FOUND: " << path << std::endl;
        return "";
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf(); // Read file buffer into stringstream
    return shaderStream.str(); // Return the string content of the shader
}


void ShaderModule::SetInt(const std::string& name, int value) const {
    glUniform1i(getUniformLocation(name), value);
}

void ShaderModule::SetFloat(const std::string& name, float value) const {
    glUniform1f(getUniformLocation(name), value);
}

void ShaderModule::SetVec2(const std::string& name, const glm::vec2& value) const {
	glUniform2fv(getUniformLocation(name), 1, &value[0]);
}

void ShaderModule::SetVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(getUniformLocation(name), 1, &value[0]);
}

void ShaderModule::SetVec4(const std::string& name, const glm::vec4& value) const {
    glUniform3fv(getUniformLocation(name), 1, &value[0]);
}

void ShaderModule::SetMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

GLint ShaderModule::getUniformLocation(const std::string& name) const {
    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location == -1) {
        std::cerr << "WARNING: Uniform '" << name << "' not found in shader!" << std::endl;
    }
    return location;
}
