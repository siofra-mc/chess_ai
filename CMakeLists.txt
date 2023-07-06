# CMakeList.txt : CMake project for chess_ai, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("chess_ai")

# Add source to this project's executable.
add_executable (chess_ai "main.cpp"  "board.h" "board.cpp"  "game.cpp" "game.h" "util.h" "triangle.h" "triangle.cpp" "hexahedron.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET chess_ai PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
find_package(GLEW CONFIG REQUIRED)
target_link_libraries(chess_ai PRIVATE GLEW::GLEW)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(chess_ai PRIVATE glfw)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(chess_ai PRIVATE imgui::imgui)