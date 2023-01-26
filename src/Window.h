#pragma once
#include <stdint.h>
#include <stdint.h>

struct ImGuiIO;
struct GLFWwindow;

class Window
{
public:
	Window() = default;
	~Window();

	bool Create(int width, int height);
	void Shutdown();
	void Update();

	inline GLFWwindow* GetGLFWWindowHandle() { return m_WindowHandle; }
		
	void GetWindowSize(int* width, int* height);
	bool GetWindowCloseFlag();
		
	ImGuiIO& GetImGuiIO();

private:
	GLFWwindow* m_WindowHandle;
};