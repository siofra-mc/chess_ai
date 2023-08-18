// chess_ai.cpp : Defines the entry point for the application.
//

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "gl/glew.h"
#include "GLFW/glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "game.h"
#include "shader.h"

using namespace std;

int main() {

 /*/ Initialize GLFW.
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
    Game game(frame_buffer_object);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Starts ImGUI frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGUI components
        ImGui::Begin("Hello world!");
        ImGui::Text("Hey window!");
        ImGui::End();

        // Clear the screen and render our game
        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        game.render();

        // Render imgui into screen
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
     
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    
    return 0;

    */


    sf::RenderWindow window(sf::VideoMode(800, 600), "Test Window");
    ImGui::SFML::Init(window);
    sf::CircleShape shape(200.f, 100);
    shape.setFillColor(sf::Color(204, 77, 5)); // Color circle
    shape.setPosition(200, 100); // Center circle

    sf::RenderTexture boardTexture;
    boardTexture.create(400, 400);
    Game game(&boardTexture);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }
        ImGui::SFML::Update(window, deltaClock.restart());

        window.clear(sf::Color(18, 33, 43)); // Color background
        window.draw(shape);
        game.render();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;

}