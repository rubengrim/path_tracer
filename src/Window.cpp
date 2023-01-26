#include "Window.h"
#include "Logger.h"

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include <stdint.h>

void ErrorCallback(int error, const char* description)
{
	PULSE_ERROR("GLFW error: {}", description)
}

Window::~Window()
{
	Shutdown();
}

bool Window::Create(int width, int height)
{
	bool ret = true;

	if (!glfwInit())
	{
		PULSE_ERROR("Error during initialization of GLFW.")
		ret = false;
	}

	glfwSetErrorCallback(ErrorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	m_WindowHandle = glfwCreateWindow(width, height, "My Title", NULL, NULL);
	if (m_WindowHandle == nullptr)
	{
		PULSE_ERROR("Error during GLFW window creation.")
		ret = false;
	}

	glfwMakeContextCurrent(m_WindowHandle);
	gladLoadGL(glfwGetProcAddress);

	glfwSwapInterval(1);

	return ret;
}

void Window::Shutdown()
{
	glfwDestroyWindow(m_WindowHandle);
	glfwTerminate();
}

void Window::Update()
{
	glfwSwapBuffers(m_WindowHandle);
	glfwPollEvents();
}

void Window::GetWindowSize(int* width, int* height)
{
	glfwGetWindowSize(m_WindowHandle, width, height);
}

bool Window::GetWindowCloseFlag()
{
	return glfwWindowShouldClose(m_WindowHandle);
}

ImGuiIO& Window::GetImGuiIO()
{
	return ImGui::GetIO();
}