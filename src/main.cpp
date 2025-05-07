#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <vector>
#include "camera.h"
#include "utilities.h"


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

    struct Hittable {
        int type;
        glm::vec3 sphereCenter;
        float sphereRadius;
    };

    std::vector<Hittable> hitObjects = {
        {0, glm::vec3(0.0f,-100.5f,-1.f), 100.f},
        {0, glm::vec3(0.0f,0.0f,-1.f), 0.5f},
    };

    // Quad rendered in vertex shader but some vao must be bound to render anything
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    shaderProgram.use();

    int locCount = glGetUniformLocation(shaderProgram.m_ProgramId, "hittableCount");
    glUniform1i(locCount, (int)hitObjects.size());

    for (int i = 0; i < hitObjects.size(); ++i) {
        // objects[i].type
        std::string base = "hittables[" + std::to_string(i) + "].";
        glUniform1i(glGetUniformLocation(shaderProgram.m_ProgramId, (base + "type").c_str()), hitObjects[i].type);
        glUniform3fv(glGetUniformLocation(shaderProgram.m_ProgramId, (base + "sphereCenter").c_str()), 1, glm::value_ptr(hitObjects[i].sphereCenter));
        glUniform1f(glGetUniformLocation(shaderProgram.m_ProgramId, (base + "sphereRadius").c_str()), hitObjects[i].sphereRadius);
    }

    // Init Camera
    Camera cam;
    cam.initialize();

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

        shaderProgram.use();          // make sure it’s still bound

        // Generate the random numbers
        set_urandom(uRandom);
        for (int i = 0; i < uRandom.size(); i++) {
            //std::cout << uRandom[i] << " ";
        }
        //std::cout << "\n";

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
