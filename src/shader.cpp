#include "Shader.h"

// Load a file into a std::string
std::string LoadFile(const char* path) {
    std::ifstream in(path);
    if (!in) {
        std::cout << "Failed to find shader include " << path << std::endl;
    }
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

void loadIncludes() {

    // List of virtual names opengfl will use for includes
    std::vector<std::pair<const char*, const char*>> includes = {
        { "/ray.glsl", "shaders/include/ray.glsl"    },
        { "/utilities.glsl", "shaders/include/utilities.glsl"    },
        { "/interval.glsl", "shaders/include/interval.glsl"    },
        { "/camera.glsl", "shaders/include/camera.glsl"    },
        { "/aabb.glsl", "shaders/include/aabb.glsl"    },

    };

    for (std::pair<const char*, const char*>& inc : includes) {
        const char* name = inc.first;
        std::string src = LoadFile(inc.second);
        glNamedStringARB(
            GL_SHADER_INCLUDE_ARB,
            -1,
            name,
            -1,
            src.c_str()
        );
    }
}

Shader::Shader(const char* vertPath, const char* fragPath, const char* geomPath)
{

    // Load shader includes
    loadIncludes();

    m_ProgramId = glCreateProgram();
    compileShader(vertPath, "VERTEX", m_ProgramId);
    compileShader(fragPath, "FRAGMENT", m_ProgramId);
    if (geomPath) compileShader(geomPath, "GEOMETRY", m_ProgramId);

    // Now that all shaders are attached, link the program:
    glLinkProgram(m_ProgramId);
    checkCompileErrors(m_ProgramId, "PROGRAM", "PROGRAM");
}

void Shader::compileShader(const char* shader_path,
    const char* type,
    unsigned int programId)
{
    std::cout << "COMPILING SHADER: " << shader_path << "\n";
    std::ifstream file(shader_path);
    if (!file) {
        std::cerr << "ERROR: Shader file not found: " << shader_path << "\n";
        std::exit(EXIT_FAILURE);
    }

    std::string source((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    const char* src = source.c_str();

    // 1) Create the right shader object
    GLuint shader = 0;
    if (strcmp(type, "VERTEX") == 0) shader = glCreateShader(GL_VERTEX_SHADER);
    else if (strcmp(type, "FRAGMENT") == 0) shader = glCreateShader(GL_FRAGMENT_SHADER);
    else if (strcmp(type, "GEOMETRY") == 0) shader = glCreateShader(GL_GEOMETRY_SHADER);
    else {
        std::cerr << "ERROR: Invalid shader type: " << type << "\n";
        return;
    }

    // 2) Compile
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    checkCompileErrors(shader, type, shader_path);

    // 3) Attach and then delete the shader object
    glAttachShader(programId, shader);
    glDeleteShader(shader);
}

Shader::Shader() = default;

void Shader::use()
{
    glUseProgram(m_ProgramId);
}

void Shader::checkCompileErrors(const unsigned int id, const std::string& type, const std::string& path)
{
    int  success;
    char infoLog[512];

    //CHeck the compilation of the shaders
    //This essentially checks for syntax errors like missing semi-colons in the fragment and vertex shaders
    if (type != "PROGRAM")
    {
        glGetShaderiv(id, GL_COMPILE_STATUS, &success); //Check compilation success
        glGetShaderInfoLog(id, 512, NULL, infoLog); //If error message exists, write it to infoLog

        if (!success)
        {
            std::cout << "SHADER::" << type << "::COMPILATION_FAILED\n" << infoLog << "\n" << path << "\n";
        }
    }

    //Check for linking success
    //Linking is the process of combining the shaders into the rendering pipeline
    //Linking ensures the output of one shader stages matches the input of the next
    //Just because compilation succeeded does not imply Linking will also succeed
    else if (type == "PROGRAM")
    {
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        glGetProgramInfoLog(id, 512, nullptr, infoLog);

        if (!success) {
            std::cout << "Program Linking Failed: " << infoLog << "\n" << path << "\n";
        }
    }
}

void Shader::setBool(const std::string& name, const bool value) const
{
    glUniform1i(checkUniformLocation(name), static_cast<int>(value));
}

void Shader::setInt(const std::string& name, const int value) const
{
    // Set the value of the uniform
    glUseProgram(m_ProgramId);
    glUniform1i(checkUniformLocation(name), value);
}

void Shader::setUInt(const std::string& name, const unsigned int value) const
{
    // Set the value of the uniform
    glUseProgram(m_ProgramId);
    glUniform1i(checkUniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, const float value) const
{
    glUniform1f(checkUniformLocation(name), value);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(checkUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat4Array(const std::string& name, const std::vector<glm::mat4>& value) const
{
    glUniformMatrix4fv(checkUniformLocation(name), value.size(), GL_FALSE, glm::value_ptr(value[0]));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(checkUniformLocation(name), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, const float x, const float y, const float z) const
{
    glUniform3f(checkUniformLocation(name), x, y, z);
}

GLint Shader::checkUniformLocation(const std::string& name) const
{
    GLint location = glGetUniformLocation(m_ProgramId, name.c_str());
    if (location == -1) {
        //std::string errorMsg = "ERROR: Uniform location for " + name + " not found in " + filePaths[0] +", " + filePaths[1];
        //std::cerr << errorMsg << "\n";
        //throw std::runtime_error(errorMsg);
    }
    return location;
}