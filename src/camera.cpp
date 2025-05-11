#include "camera.h"

unsigned int Camera::SCR_WIDTH = 1280;
unsigned int Camera::SCR_HEIGHT = 720;

Camera::Camera() {

    m_LookFrom = glm::vec3(-2, 2, 1);;
    m_LookAt   = glm::vec3(0, 0, -1);
    m_Up      = glm::vec3(0, 1, 0);
    m_Fov    = 60;

    m_Yaw = -90.0f;
    m_Pitch = 0.0f;
}

void Camera::setUniforms(GLuint program_id)
{
    //TODO: Clean this up. program.used called before setunfiroms is called 
    //m_LookAt += 0.0001;
    std::string base = "cam.";
    glUniform3fv(glGetUniformLocation(program_id, (base + "lookfrom").c_str()), 1, glm::value_ptr(m_LookFrom));
    glUniform3fv(glGetUniformLocation(program_id, (base + "lookat").c_str()), 1, glm::value_ptr(m_LookAt));
    glUniform3fv(glGetUniformLocation(program_id, (base + "vup").c_str()), 1, glm::value_ptr(m_Up));
    glUniform1f(glGetUniformLocation(program_id, (base + "vfov").c_str()), m_Fov);
}

void Camera::processMouse(double xoffset, double yoffset)
{
    m_Yaw += xoffset;
    m_Pitch += yoffset;

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
    //printf("Process keyboard called!\n");
    glm::vec3 front = glm::normalize(m_LookAt - m_LookFrom);
    glm::vec3 right = glm::normalize(cross(front, m_Up));

    // delta is provided as a double from GLFW so safely cast it to float
    float d = static_cast<float>(delta);

    if (key == GLFW_KEY_W)       m_LookFrom += front * d;
    else if (key == GLFW_KEY_S)  m_LookFrom -= front * d;
    else if (key == GLFW_KEY_D)  m_LookFrom += right * d;
    else if (key == GLFW_KEY_A)  m_LookFrom -= right * d;
    else if (key == GLFW_KEY_SPACE)         m_LookFrom.y -= d;
    else if (key == GLFW_KEY_LEFT_CONTROL)  m_LookFrom.y += d;

    m_LookAt = m_LookFrom + front;

}
