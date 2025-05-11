#pragma once

#include <string>
#include "glm/glm.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    Camera();
    void setUniforms(GLuint program_id);
    void processMouse(float xoffset, float yoffset);
    void processKeyboard(float changeValue, unsigned int keyPressed);
    glm::vec3 m_LookFrom; // Cam location
    glm::vec3 m_LookAt;// Look at location
    glm::vec3 m_Up;// What is considered up
    float m_Fov;
    float m_Yaw;
    float m_Pitch;
    //float m_MovementSpeed;
    //float m_MouseSensitivity;

    static unsigned int SCR_WIDTH;
    static unsigned int SCR_HEIGHT;

private:

};


