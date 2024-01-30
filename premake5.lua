workspace "ChessAI"
	configurations { "Debug", "Release" }
	platforms { "Win32", "Win64" }

project "ChessAI"

	kind "ConsoleApp"
	language "C++"

	outputdir = "out/%cfg.{buildcfg}"
	
	targetdir (outputdir)

	files {
		"src/**.h",
		"src/**.cpp",
		"premake5.lua"
	}

	defines { "GLEW_STATIC" }

	includedirs {
		"lib/imgui",
		"lib/imgui/backends",
		"lib/glm",
		"lib/glew/include",
		"lib/glfw/include"
	}

	include "lib/imgui"

	links {"opengl32.lib", "ImGui", "GLFW"}

	

	filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

    filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

	filter "platforms:Win32"
	  system "Windows"
	  architecture "x86"
	  libdirs { "lib/glew/lib/Release/Win32" }
	  links {"glew32.lib"}

	filter "platforms:Win64"
	  system "Windows"
	  architecture "x86_64"
	  libdirs { "lib/glew/lib/Release/x64" }
	  links {"glew32.lib"}