#include "camera.h"

Camera::Camera() {

    m_LookFrom = glm::vec3(-2, 2, 1);;
    m_LookAt   = glm::vec3(0, 0, -1);
    m_Up      = glm::vec3(0, 1, 0);
    m_Fov    = 20;
}

void Camera::setUniforms(GLuint program_id)
{
    //used called before setunfiroms is called
    m_LookAt += 0.0001;
    std::string base = "cam.";
    glUniform3fv(glGetUniformLocation(program_id, (base + "lookfrom").c_str()), 1, glm::value_ptr(m_LookFrom));
    glUniform3fv(glGetUniformLocation(program_id, (base + "lookat").c_str()), 1, glm::value_ptr(m_LookAt));
    glUniform3fv(glGetUniformLocation(program_id, (base + "vup").c_str()), 1, glm::value_ptr(m_Up));
    glUniform1f(glGetUniformLocation(program_id, (base + "vfov").c_str()), m_Fov);
}
