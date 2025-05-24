#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <gl/GL.h>

class Shader
{
public:
    // the program ID
    unsigned int m_ProgramId;
    std::vector<std::string> filePaths;


    // constructor reads and builds the shader
    Shader(const char* vertex_path, const char* fragment_path, const char* geometry_path = nullptr);
    Shader();
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string& name, const bool value) const;
    void setInt(const std::string& name, const int value) const;
    void setUInt(const std::string& name, const unsigned int value) const;
    void setFloat(const std::string&, const float value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;
    void setMat4Array(const std::string& name, const std::vector<glm::mat4>& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;

private:
    void checkCompileErrors(const unsigned int id, const std::string& type, const std::string& path);
    void compileShader(const char* shader_path, const char* type, unsigned int program_id);
    GLint checkUniformLocation(const std::string& name) const;
};