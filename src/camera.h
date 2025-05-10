#pragma once

#include <string>
#include "glm/glm.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

class Camera {
public:
    Camera();
    void setUniforms(GLuint program_id);
    glm::vec3 m_LookFrom; // Cam location
    glm::vec3 m_LookAt;// Look at location
    glm::vec3 m_Up;// What is considered up
    float m_Fov    = 20;

private:

};