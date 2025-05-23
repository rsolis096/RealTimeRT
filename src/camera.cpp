#include "camera.h"

unsigned int Camera::SCR_WIDTH = 1280;
unsigned int Camera::SCR_HEIGHT = 720;

Camera::Camera() {

    m_LookFrom = glm::vec3(13, 2, 3);
    m_LookAt   = glm::vec3(0, 0, 0);
    m_Up      = glm::vec3(0, 1, 0);
    m_Fov    = 20.f;

    // Initializes Pitch and Yaw to match the direction we should be looking
    glm::vec3 dir = glm::normalize(m_LookAt - m_LookFrom);
    m_Pitch = glm::degrees(asin(dir.y));
    m_Yaw = glm::degrees(atan2(dir.z, dir.x));

    m_DefocusAngle = 0.6f;
    m_FocusDist = 10.;
}

void Camera::setUniforms(GLuint program_id)
{
    glUseProgram(program_id);
    std::string base = "cam.";
    glUniform3fv(glGetUniformLocation(program_id, (base + "lookfrom").c_str()), 1, glm::value_ptr(m_LookFrom));
    glUniform3fv(glGetUniformLocation(program_id, (base + "lookat").c_str()), 1, glm::value_ptr(m_LookAt));
    glUniform3fv(glGetUniformLocation(program_id, (base + "vup").c_str()), 1, glm::value_ptr(m_Up));
    glUniform1f(glGetUniformLocation(program_id, (base + "vfov").c_str()), m_Fov);
    glUniform1f(glGetUniformLocation(program_id, (base + "defocus_angle").c_str()), m_DefocusAngle);
    glUniform1f(glGetUniformLocation(program_id, (base + "focus_dist").c_str()), m_FocusDist);

}

void Camera::processMouse(double xoffset, double yoffset)
{

    m_Yaw += xoffset;
    m_Pitch += yoffset;

    // Prevents pitch from flipping the image
    if (m_Pitch > 89.0f)
        m_Pitch = 89.0f;
    if (m_Pitch < -89.0f)
        m_Pitch = -89.0f;
    
    m_LookAt.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_LookAt.y = sin(glm::radians(m_Pitch));
    m_LookAt.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_LookAt = m_LookFrom + glm::normalize(m_LookAt);
    
}

void Camera::processKeyboard(double delta, unsigned int key)
{
    glm::vec3 front = glm::normalize(m_LookAt - m_LookFrom);
    glm::vec3 right = glm::normalize(cross(front, m_Up));

    float d = static_cast<float>(delta);

    if (key == GLFW_KEY_W)       m_LookFrom += front * d;
    else if (key == GLFW_KEY_S)  m_LookFrom -= front * d;
    else if (key == GLFW_KEY_D)  m_LookFrom += right * d;
    else if (key == GLFW_KEY_A)  m_LookFrom -= right * d;
    else if (key == GLFW_KEY_SPACE)         m_LookFrom.y += d;
    else if (key == GLFW_KEY_LEFT_CONTROL)  m_LookFrom.y -= d;

    m_LookAt = m_LookFrom + front;

}


void Camera::processCameraInput(GLFWwindow* window, double deltaTime) {

    // Forward
    double cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        processKeyboard(cameraSpeed, GLFW_KEY_W);
    // Backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        processKeyboard(cameraSpeed, GLFW_KEY_S);
    // Left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        processKeyboard(cameraSpeed, GLFW_KEY_A);
    // Right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        processKeyboard(cameraSpeed, GLFW_KEY_D);

    // Up
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        processKeyboard(deltaTime, GLFW_KEY_SPACE);
    // Down
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        processKeyboard(deltaTime, GLFW_KEY_LEFT_CONTROL);

}