#define GLM_ENABLE_EXPERIMENTAL



#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#include <vector>


#include "shader.h"
#include "camera.h"
#include "utilities.h"

#include "Hittable.h"
#include "Sphere.h"
#include "Cube.h"

#include "GUI.h"

Camera cam;
double deltaTime = 0.0;
double debounceThreshold = 0.2;
bool cursorLocked = true;
double lastKeyPressTime = 0.0;

// Mouse location properties
double lastX;
double lastY;

//Scene Setup
std::vector<GPUHittable>  gpuPrims;
std::vector<GPUSphere>    gpuSpheres;
std::vector<GPUCube>      gpuCubes;
std::vector<GPUMaterial>  gpuMats;
GLuint ssboPrims = 0, ssboSpheres = 0, ssboCubes = 0, ssboMats = 0;

void mouse_callback(GLFWwindow* window, double mouse_x, double mouse_y)
{

    // Compute the change in mouse position
    double delta_x = mouse_x - lastX;
    double delta_y = lastY - mouse_y; // reversed since y-coordinates go from bottom to top

    lastX = mouse_x;
    lastY = mouse_y;

    const double sensitivity = 0.3;
    delta_x *= sensitivity;
    delta_y *= sensitivity;

    if(!isWindowHidden)
        cam.processMouse(delta_x, delta_y);
}

void processInput(GLFWwindow* window, double deltaTime) {

    if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS)
    {
        double currentTime = glfwGetTime();
        if (currentTime - lastKeyPressTime > debounceThreshold)
        {
            isWindowHidden = !isWindowHidden;
            if (cursorLocked)
            {
                //Unlock mouse
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                cursorLocked = false;

            }
            else
            {
                //Place mouse on middle of screen
                glfwSetCursorPos(window, Camera::SCR_WIDTH / 2,Camera::SCR_HEIGHT/2);
                //Lock mouse
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                cursorLocked = true;

            }
            lastKeyPressTime = currentTime; // Update debounce timer
        }
    }

    // End program
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Camera::SCR_WIDTH = width;
    Camera::SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

void setup_scene() {

    /*
    Fill these vectors

    std::vector<GPUHittable>  gpuPrims;
    std::vector<GPUSphere>    gpuSpheres;
    std::vector<GPUCube>      gpuCubes;
    std::vector<GPUMaterial>     gpuMats;
    
    */

    // Ground
    Material ground_material = Material::MakeLambertian(glm::vec3(0.5, 0.5, 0.5));

    // albedo, fuzz, type, refraction, padding
    gpuMats.push_back({ { ground_material.m_Albedo, ground_material.m_Fuzz }, {float(ground_material.m_Type), 0.f, 0.f, 0.f} });
    // position, radius, color, mat index
    gpuSpheres.push_back({ {0, -1000.f, 0.f, 1000.f}, {0.f, 0.f, 0.f, 0.f } });
        
    // Generate objects with random materials
    for (int a = -4; a < 4; a++) {
        for (int b = -4; b < 4; b++) {


            float choose_mat = random_float();
            glm::vec3 center(a + 0.9 * random_float(), 0.2, b + 0.9 * random_float());

            if ((center - glm::vec3(4, 0.2, 0)).length() > 0.9) {

                if (choose_mat < 0.8) {

                    // diffuse
                    glm::vec3 albedo = random_vec() * random_vec();
                    Material sphere_material = Material::MakeLambertian(albedo);
                    gpuMats.push_back({ { sphere_material.m_Albedo, sphere_material.m_Fuzz }, {float(sphere_material.m_Type), 0.f, 0.f, 0.f} });
                    gpuSpheres.push_back({ {center, 0.2f}, {0.f, 0.f, 0.f, float(gpuMats.size() - 1)} });

                }
                else if (choose_mat < 0.95) {

                    // metal
                    glm::vec3 albedo = random_vec(0.5, 1);
                    float fuzz = random_float(0, 0.5);
                    Material sphere_material = Material::MakeMetal(albedo, fuzz);
                    gpuMats.push_back({ { sphere_material.m_Albedo, sphere_material.m_Fuzz }, {float(sphere_material.m_Type), 0.f, 0.f, 0.f} });
                    gpuSpheres.push_back({ {center, 0.2f}, {0.f, 0.f, 0.f, float(gpuMats.size() - 1)} });

                }
                else {

                    // glass
                    Material sphere_material = Material::MakeDielectric(1.5);
                    gpuMats.push_back({ { sphere_material.m_Albedo, sphere_material.m_Fuzz }, {float(sphere_material.m_Type), 0.f, 0.f, 0.f} });
                    gpuSpheres.push_back({ {center, 0.2f}, {0.f, 0.f, 0.f, float(gpuMats.size() - 1)} });
                }
            }
        }
    }

    
    Material material1 = Material::MakeDielectric(1.5);
    gpuMats.push_back({ { material1.m_Albedo, material1.m_Fuzz }, {float(material1.m_Type), 0.f, 0.f, 0.f} });
    gpuSpheres.push_back({ {0, 1.f, 0.f, 1.f}, {0.f, 0.f, 0.f, float(gpuMats.size() - 1) } });

    Material material2 = Material::MakeLambertian(glm::vec3(0.4, 0.2, 0.1));
    gpuMats.push_back({ { material2.m_Albedo, material2.m_Fuzz }, {float(material2.m_Type), 0.f, 0.f, 0.f} });
    gpuSpheres.push_back({ {-4.f, 1.f, 0.f, 1.f}, {0.f, 0.f, 0.f, float(gpuMats.size() - 1) } });

    Material material3 = Material::MakeMetal(glm::vec3(0.7, 0.6, 0.5), 0.0);
    gpuMats.push_back({ { material3.m_Albedo, material3.m_Fuzz }, {float(material3.m_Type), 0.f, 0.f, 0.f} });
    gpuSpheres.push_back({ {4.f, 1.f, 0.f, 1.f}, {0.f, 0.f, 0.f,  float(gpuMats.size() - 1)} });


}

void upload_ssbo(GLuint& id, GLuint binding, const void* data, GLsizeiptr bytes) {
    glGenBuffers(1, &id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bytes, data, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
};


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
    GLFWwindow* window = glfwCreateWindow(Camera::SCR_WIDTH, Camera::SCR_HEIGHT, "OpenGL Version Check", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    // Initialize cursor position to prevent snapping
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        glfwSetCursorPos(window, width * 0.5, height * 0.5);

        lastX = width * 0.5;
        lastY = height * 0.5;

    }

    // Remove vsync
    glfwSwapInterval(0);

    // Locks cursor to screen, apply callback function
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Used to update dynamically update window size
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    // Initialize GLAD
    // 
    // Load OpenGL functions using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // Check if extension is available
    if (!glfwExtensionSupported("GL_ARB_shading_language_include")) {
        std::cerr << "Include extension not available!\n";
        return -1;
    }

    // Specify viewport of OpenGL
    glViewport(0, 0, Camera::SCR_WIDTH, Camera::SCR_HEIGHT);

    GLuint imageTexture;
    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, Camera::SCR_WIDTH, Camera::SCR_HEIGHT);



    // Create Shader program
    Shader graphicsProgram("shaders/source/vert.glsl", "shaders/source/frag.glsl");
    Shader computeProgram("shaders/source/comp.glsl");

    // Quad rendered in vertex shader but some vao must be bound to render anything
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create the scene
    setup_scene();

    // Send scene to computer shader (upload ssbo and  init key values
    computeProgram.use();
    upload_ssbo(ssboSpheres, /*binding=*/0, gpuSpheres.data(), gpuSpheres.size() * sizeof(GPUSphere));
    upload_ssbo(ssboMats, /*binding=*/1, gpuMats.data(), gpuMats.size() * sizeof(GPUMaterial));
    glUniform1i(glGetUniformLocation(computeProgram.m_ProgramId, "uSphereCount"), (int)gpuSpheres.size());
    glUniform1i(glGetUniformLocation(computeProgram.m_ProgramId, "uMaterialsCount"), (int)gpuMats.size());
    computeProgram.setInt("SCR_HEIGHT", Camera::SCR_HEIGHT);
    computeProgram.setInt("SCR_WIDTH", Camera::SCR_WIDTH);

    // Set uniforms for the graphics (fragment) program
    graphicsProgram.use();
    graphicsProgram.setInt("uOutputTexture", 0);

    // Print version info
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version:   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor:         " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer:       " << glGetString(GL_RENDERER) << std::endl;
     
    // Initialize ImGui
    init_gui(window);

    double delay_fps_display = 0.0f;
    float display_fps = 1.f;

    // Draw Loop
    while (!glfwWindowShouldClose(window)) {

        // Frame rate
        double frameStart = glfwGetTime();

        // Update Compute Shader
        computeProgram.use();
        computeProgram.setFloat("uSeed", random_float());
        computeProgram.setInt("SAMPLES", number_of_samples);
        computeProgram.setInt("MAX_DEPTH", ray_depth);
        cam.setUniforms(computeProgram.m_ProgramId);

        // Dispatch the compute workgroups (this groups sizing performs better)
        glDispatchCompute(
            (GLuint)ceil(Camera::SCR_WIDTH / 16.0),
            (GLuint)ceil(Camera::SCR_HEIGHT / 16.0),
            1
        );
        glMemoryBarrier(GL_ALL_BARRIER_BITS);


        // Clear the background
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw texture using vertex and fragment shader
        graphicsProgram.use();
        glBindImageTexture(0, imageTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // Display DearImGui
        display_gui(display_fps);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Compute frame time
        double frameEnd = glfwGetTime();
        deltaTime = frameEnd - frameStart;

        // Delay fps updates by 1 second
        delay_fps_display += deltaTime;
        if (delay_fps_display > 1) {
            display_fps = deltaTime;
            delay_fps_display = 0.1f;
        }


        if(!isWindowHidden)
            cam.processCameraInput(window, deltaTime);
        processInput(window, deltaTime);
    }

    // Cleanup
    glDeleteProgram(graphicsProgram.m_ProgramId);
    glDeleteProgram(computeProgram.m_ProgramId);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);

    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
