project "glad"
	kind "StaticLib"
	architecture "x86_64"
	language "C"

	configurations
	{
		"Debug",
		"Release"
	}

	targetdir "bin/%{cfg.buildcfg}"
	objdir "obj/%{cfg.buildcfg}"
	
	includedirs { "include" }

	files
	{
		"src/gl.c",
		"include/glad/gl.h"
	}

	filter ("system:windows")
		systemversion "latest"

	filter ("configurations:Debug")
		runtime "Debug"
		symbols "on"

	filter ("configurations:Release")
		runtime "Release"
		optimize "on"