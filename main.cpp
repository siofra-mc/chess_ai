// chess_ai.cpp : Defines the entry point for the application.
//

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "gl/glew.h"
#include "GLFW/glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "game.h"
#include "shader.h"
#include <vector>

using namespace std;


void render(GLFWwindow* window, Game* g) {
    glfwPollEvents();

    // Starts ImGUI frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    // Clear the screen and render our game
    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
    g->render();

    // Render imgui into screen
    ImGui::Render();
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}


 
int main() {
    // Initialize GLFW.
    if (!glfwInit())
    {
        cerr << "Failed to initialize GLFW" << endl;
        return NULL; 
    }

    // Create the GLFW window.
    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE, NULL, NULL);
    if (!window) 
    {
        cerr << "Failed to open GLFW window." << endl;
        glfwTerminate(); 
        exit(EXIT_FAILURE);
    }

    // Make the context of the window.
    glfwMakeContextCurrent(window);

    
    // Initialize GLEW.
    if (glewInit())
    {
        cerr << "Failed to initialize GLEW" << endl;
        return NULL;
    }
    
    glfwSwapInterval(0);
        
    // ImGUI init
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    ImGui::StyleColorsDark();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    // Initialization for our program's graphical components
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    unsigned int frame_buffer_object;
    glGenFramebuffers(1, &frame_buffer_object);

    // Let's get this game going!
    Game* game = new Game(frame_buffer_object);
    while (!glfwWindowShouldClose(window)) {
        render(window, game);
    }
     
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    
    return 0;
}