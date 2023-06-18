// chess_ai.cpp : Defines the entry point for the application.
//

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "gl/glew.h"
#include "GLFW/glfw3.h"
#include "game.h"

using namespace std;

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

    // Set swap interval to 1.
    glfwSwapInterval(0);
        
    // imgui init
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    ImGui::StyleColorsDark();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    

    const char* vertexShaderSource = "#version 330 core\n"
        "layout(location = 0) in vec3 pos;\n"
        "layout(location = 1) in vec3 col;\n"
        "out vec4 vertColor;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);\n"
        "   vertColor   = vec4(col.x, col.y, col.z, 1.0);\n"
        "}\0";

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);    
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    const char* fragmentShaderSource = "#version 330 core\n"
        "in vec4 vertColor;\n"
        "out vec4 FragColor; \n"
        "void main()\n"
        "{\n"
        "   FragColor = vertColor;\n"
        "}\0";

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
            -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
             0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f };

    unsigned int frame_buffer_object;
    glGenFramebuffers(1, &frame_buffer_object);
    //glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_object);

    glBindVertexArray(0);

    Game game(frame_buffer_object);
    while (!glfwWindowShouldClose(window)) {
        //cout << ImGui::GetWindowSize().x << ", " << ImGui::GetWindowSize().y << endl;
        
        glfwPollEvents();

        // starts imgui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // imgui components
        ImGui::Begin("Hello world!");
        ImGui::Text("Hey window!");
        ImGui::End();

        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);

        game.render(shaderProgram);

        // Render imgui into screen
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
     
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteProgram(shaderProgram);

    //board.play();

    glfwTerminate();
    return 0;
}