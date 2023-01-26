#pragma once
#include "Logger.h"
#include "Window.h"
#include "Image.h"
#include "Camera.h"
#include "Scene.h" 

#include <chrono>

typedef unsigned int GLuint;

class Application
{
public:
	Application() = default;
	~Application();
		
	bool Initialize();
	void Run();
	void Shutdown();

	bool InitializeImGui();

private:
	Logger m_Logger;
	Window m_Window;
	
	Camera* m_Camera = nullptr;
	Image* m_Image = nullptr;

	int m_ViewportWidth = 0;
	int m_ViewportHeight = 0;

	bool m_IsRunning = false;

	std::chrono::time_point<std::chrono::steady_clock> m_BeginFrame;
	std::chrono::time_point<std::chrono::steady_clock> m_EndFrame;
};
