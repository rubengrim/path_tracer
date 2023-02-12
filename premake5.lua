workspace "Pulse"
	architecture "x86_64"

	configurations
	{
		"Debug",
		"Release"	
	}

project "Pulse"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/%{cfg.buildcfg}-%{cfg.architecture}")
	objdir ("obj/%{cfg.buildcfg}-%{cfg.architecture}")

	files 
	{
		"src/**.cpp",
		"src/**.h",
		"external/imgui/imgui/*.cpp",
		"external/imgui/imgui/*.h",
		"external/imgui/misc/cpp/*.cpp",
		"external/imgui/misc/cpp/*.h",
		"external/imgui/misc/fonts/*.ttf",
		"external/imgui/imgui/backends/imgui_impl_glfw.cpp",
		"external/imgui/imgui/backends/imgui_impl_glfw.h",
		"external/imgui/imgui/backends/imgui_impl_opengl3.cpp",
		"external/imgui/imgui/backends/imgui_impl_opengl3.h"
	}

	includedirs
	{
		"external/imgui",
		"external/imgui/misc/cpp",
		"external/spdlog/include",
		"external/GLFW/include",
		"external/glad/include",
		"external/eigen/include"
	}

	links
	{
		"GLFW",
		"glad"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		
		defines
		{
			"PULSE_DEBUG"
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		
		defines
		{
			"PULSE_RELEASE"
		}

    filter "system:windows"
        systemversion "latest"

include "external/GLFW/glfw.lua"
include "external/glad/glad.lua"