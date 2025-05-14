#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

static bool isWindowHidden = false;
static int number_of_samples = 5;
static int ray_depth = 10;

void display_gui(double deltaTime) {

    if (isWindowHidden) {


        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Rendering
        ImGui::Begin("OpenGL Project");
        ImGui::Text("%.3f fps", (1.f / static_cast<float>(deltaTime)));

        ImGui::Text("# of Samples");
        ImGui::DragInt("##samples", &number_of_samples, 1.f, 1, 5);

        ImGui::Text("Ray Depth");
        ImGui::DragInt("##ray_depth", &ray_depth, 1.f, 1, 10);

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    }

}

void init_gui(GLFWwindow* window) {


    // Setup ImGUI context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    //GUI::isWindowHidden = true;
}
