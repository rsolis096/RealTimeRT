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

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

Camera cam;
double deltaTime = 0.0;

// Mouse location properties
double lastX;
double lastY;
bool firstMouse = true;

void init_gui(GLFWwindow* window) {

    // Setup ImGUI context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); //Input/Output allowing user and gui interactions

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    //GUI::isWindowHidden = true;
}

void display_gui() {

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Rendering
    // (Your code clears your framebuffer, renders your other stuff etc.)
    ImGui::Begin("OpenGL Project");
    ImGui::Text("%.3f fps", (1.f / deltaTime));

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

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

    cam.processMouse(delta_x, delta_y);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Camera::SCR_WIDTH = width;
    Camera::SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

void setup_scene(std::vector<Hittable>& hitObjects) {

    Material ground_material = Material::MakeLambertian(glm::vec3(0.5, 0.5, 0.5));
    hitObjects.push_back(Hittable(glm::vec3(0, -1000, 0), 1000, ground_material));

    for (int a = -4; a < 4; a++) {
        for (int b = -4; b < 4; b++) {
            float choose_mat = random_float();
            glm::vec3 center(a + 0.9 * random_float(), 0.2, b + 0.9 * random_float());

            if ((center - glm::vec3(4, 0.2, 0)).length() > 0.9) {

                if (choose_mat < 0.8) {
                    // diffuse
                    glm::vec3 albedo = random_vec() * random_vec();
                    Material sphere_material = Material::MakeLambertian(albedo);
                    hitObjects.push_back(Hittable(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    glm::vec3 albedo = random_vec(0.5, 1);
                    float fuzz = random_float(0, 0.5);
                    Material sphere_material = Material::MakeMetal(albedo, fuzz);
                    hitObjects.push_back(Hittable(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    Material sphere_material = Material::MakeDielectric(1.5);
                    hitObjects.push_back(Hittable(center, 0.2, sphere_material));
                }
            }
        }
    }

    Material material1 = Material::MakeDielectric(1.5);
    hitObjects.push_back(Hittable(glm::vec3(0, 1, 0), 1.0, material1));

    Material material2 = Material::MakeLambertian(glm::vec3(0.4, 0.2, 0.1));
    hitObjects.push_back(Hittable(glm::vec3(-4, 1, 0), 1.0, material2));

    Material material3 = Material::MakeMetal(glm::vec3(0.7, 0.6, 0.5), 0.0);
    hitObjects.push_back(Hittable(glm::vec3(4, 1, 0), 1.0, material3));
}

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

    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // (2) Warp to center:
        glfwSetCursorPos(window, width * 0.5, height * 0.5);

        // (3) Now initialize your lastX/lastY from the same center:
        lastX = width * 0.5;
        lastY = height * 0.5;

    }

    // Locks cursor to screen, apply callback function
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Used to update dynamically update window size
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load OpenGL functions using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // Specify viewport of OpenGL
    glViewport(0, 0, Camera::SCR_WIDTH, Camera::SCR_HEIGHT);

    // Create Shader program
    Shader shaderProgram("shaders/vert.glsl", "shaders/frag.glsl", NULL);

    // Quad rendered in vertex shader but some vao must be bound to render anything
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create the scene
    std::vector<Hittable> hitObjects;
    setup_scene(hitObjects);

    // Apply constant uniforms
    shaderProgram.use();

    // Send scene to fragment shader
    glUniform1i(glGetUniformLocation(shaderProgram.m_ProgramId, "hittableCount"), (int)hitObjects.size());
    for (int i = 0; i < hitObjects.size(); ++i) {
        // objects[i].type
        std::string base = "hittables[" + std::to_string(i) + "].";
        glUniform3fv(glGetUniformLocation(shaderProgram.m_ProgramId, (base + "sphereCenter").c_str()), 1, glm::value_ptr(hitObjects[i].m_Position));
        glUniform1f(glGetUniformLocation(shaderProgram.m_ProgramId, (base + "sphereRadius").c_str()), hitObjects[i].m_Radius);
        glUniform1i(glGetUniformLocation(shaderProgram.m_ProgramId, (base + "mat.type").c_str()), hitObjects[i].m_Mat.m_Type);
        glUniform3fv(glGetUniformLocation(shaderProgram.m_ProgramId, (base + "mat.albedo").c_str()), 1, glm::value_ptr(hitObjects[i].m_Mat.m_Albedo));
        glUniform1f(glGetUniformLocation(shaderProgram.m_ProgramId, (base + "mat.refraction_index").c_str()), hitObjects[i].m_Mat.m_RefractionIndex);
        glUniform1f(glGetUniformLocation(shaderProgram.m_ProgramId, (base + "mat.fuzz").c_str()), hitObjects[i].m_Mat.m_Fuzz);
    }
    shaderProgram.setInt("SCR_HEIGHT", Camera::SCR_HEIGHT);
    shaderProgram.setInt("SCR_WIDTH", Camera::SCR_WIDTH);

    // Print version info
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version:   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor:         " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer:       " << glGetString(GL_RENDERER) << std::endl;
     

    // Initialize ImGui
    init_gui(window);

    // Draw Loop
    while (!glfwWindowShouldClose(window)) {
            

        // Frame rate
        double frameStart = glfwGetTime();

        // Background Color
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram.use();

        // Generate a random seed for the shader
        shaderProgram.setFloat("uSeed", random_float());

        // Update Camera uniform values
        cam.setUniforms(shaderProgram.m_ProgramId);

        // VAO is needed even though it does nothing
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // Display DearImGui
        display_gui();

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Compute frame time
        double frameEnd = glfwGetTime();
        deltaTime = frameEnd - frameStart;
        cam.processInput(window, deltaTime);
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
