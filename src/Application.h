#pragma once
#include "Logger.h"
#include "Image.h"
#include "Camera.h"
#include "Scene.h" 
#include "Renderer.h"

#include <chrono>

struct GLFWwindow;

class Application
{
public:
	Application() = default;
	~Application();
		
	bool Initialize();
	void Run();
	void Shutdown();

private:
	GLFWwindow* m_WindowHandle;

	Logger m_Logger;
	Camera* m_Camera = nullptr;
	Image* m_Image = nullptr;
	Renderer* m_Renderer = nullptr;

	int m_ViewportWidth = 0;
	int m_ViewportHeight = 0;

	bool m_IsRunning = false;
};
