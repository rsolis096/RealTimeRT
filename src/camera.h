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
    void processMouse(double xoffset, double yoffset);
    void processKeyboard(double delta, unsigned int key);
    glm::vec3 m_LookFrom; // Cam location
    glm::vec3 m_LookAt;// Look at location
    glm::vec3 m_Up;// What is considered up
    float m_Fov;
    float m_Yaw;
    float m_Pitch;
    float m_DefocusAngle;
    float m_FocusDist;

    static unsigned int SCR_WIDTH;
    static unsigned int SCR_HEIGHT;

    // Camera Input
    void processInput(GLFWwindow* window, double deltaTime);

};


