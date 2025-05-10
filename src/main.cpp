#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>


#include "shader.h"
#include "camera.h"
#include "utilities.h"
#include "Hittable.h"


int main() {


    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Request OpenGL 4.3 Core Profile context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Version Check", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Load OpenGL functions using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // Specify viewport of OpenGL
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);


    // Shader
    Shader shaderProgram("shaders/vert.glsl", "shaders/frag.glsl", NULL);

    // Create Spheres
    std::vector<Hittable> hitObjects = {
                {
            glm::vec3(-1.0f,   0.0f, -1.0f),
            glm::vec3(0.8f,   0.8f,  0.8f),
             0.5f, (1.5),
            DIELECTRIC
        },
        {
            glm::vec3(0.0f, -100.5f, -1.0f),  // position
            glm::vec3(0.8f,   0.8f,  0.0f),  // albedo
            100.0f,  1,                        // radius
            LAMBERTIAN                          // material
        },
        {
            glm::vec3(0.0f,   0.0f, -1.2f),
            glm::vec3(0.1f,   0.2f,  0.5f),
             0.5f, 1,
            LAMBERTIAN
        },

        {
            glm::vec3(1.0f,   0.0f, -1.0f),
            glm::vec3(0.8f,   0.6f,  0.2f),
             0.5f, 1,
            METAL, 0.3f
        },

        {
            glm::vec3(-1.0,    0.0, -1.0),
            glm::vec3(0.8f,   0.6f,  0.2f),
             0.4f, (1.00 / 1.50),
            DIELECTRIC
        }
    };

    // Setup Camera
    Camera cam;
    
    // Quad rendered in vertex shader but some vao must be bound to render anything
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    shaderProgram.use();

    cam.setUniforms(shaderProgram.m_ProgramId);

    // Apply constant uniforms
    int locCount = glGetUniformLocation(shaderProgram.m_ProgramId, "hittableCount");
    glUniform1i(locCount, (int)hitObjects.size());
    for (int i = 0; i < hitObjects.size(); ++i) {
        // objects[i].type
        std::string base = "hittables[" + std::to_string(i) + "].";
        glUniform3fv(glGetUniformLocation(shaderProgram.m_ProgramId, (base + "sphereCenter").c_str()), 1, glm::value_ptr(hitObjects[i].m_Position));
        glUniform1f(glGetUniformLocation(shaderProgram.m_ProgramId, (base + "sphereRadius").c_str()), hitObjects[i].m_Radius);
        glUniform1i(glGetUniformLocation(shaderProgram.m_ProgramId, (base + "mat.type").c_str()), hitObjects[i].m_Type);
        glUniform3fv(glGetUniformLocation(shaderProgram.m_ProgramId, (base + "mat.albedo").c_str()), 1, glm::value_ptr(hitObjects[i].m_Albedo));
        glUniform1f(glGetUniformLocation(shaderProgram.m_ProgramId, (base + "mat.refraction_index").c_str()), hitObjects[i].m_Refraction_Index);
        glUniform1f(glGetUniformLocation(shaderProgram.m_ProgramId, (base + "mat.fuzz").c_str()), hitObjects[i].m_Fuzz);
    }
    shaderProgram.setInt("SCR_HEIGHT", SCR_HEIGHT);
    shaderProgram.setInt("SCR_WIDTH", SCR_WIDTH);

    // Init Camera
    //Camera cam;
    //cam.initialize();

    // Print version info
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version:   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor:         " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer:       " << glGetString(GL_RENDERER) << std::endl;

    double previousTime = glfwGetTime();

    // Keep window open until closed

    std::vector<float> uRandom = {0,0,0,0};

    while (!glfwWindowShouldClose(window)) {
            

        // Measure speed
        double startTime = glfwGetTime();

        // Background Color
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram.use();
        // Generate a random seed for the shader
        float seed = random_float();
        shaderProgram.setFloat("uSeed",  seed);
        cam.setUniforms(shaderProgram.m_ProgramId);


        glBindVertexArray(vao);       // bind the VAO
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);


        glfwSwapBuffers(window);
        glfwPollEvents();

        double endTime = glfwGetTime();

        //std::cout << (int)(1 / (endTime - startTime)) << "\r";

    }

    // Cleanup
    glDeleteProgram(shaderProgram.m_ProgramId);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);

    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
