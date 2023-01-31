#include "Mouse.h"

#include "GLFW/glfw3.h"

GLFWwindow* Mouse::m_Window = nullptr;

double Mouse::m_X = 0;
double Mouse::m_Y = 0;
double Mouse::m_XLast = 0;
double Mouse::m_YLast = 0;

std::array<bool, Mouse::m_ButtonCount> Mouse::m_ButtonsPressed;
std::array<bool, Mouse::m_ButtonCount> Mouse::m_ButtonsDown;
std::array<bool, Mouse::m_ButtonCount> Mouse::m_ButtonsUp;

void Mouse::Initialize(GLFWwindow* window)
{
	m_Window = window;

	std::fill(m_ButtonsPressed.begin(), m_ButtonsPressed.end(), false);
	std::fill(m_ButtonsDown.begin(), m_ButtonsDown.end(), false);
	std::fill(m_ButtonsUp.begin(), m_ButtonsUp.end(), false);
}

void Mouse::Update()
{
	std::fill(m_ButtonsDown.begin(), m_ButtonsDown.end(), false);
	std::fill(m_ButtonsUp.begin(), m_ButtonsUp.end(), false);

	m_XLast = m_X;
	m_YLast = m_Y;
	glfwGetCursorPos(m_Window, &m_X, &m_Y);

	for (int i = 0; i < m_ButtonCount; i++)
	{
		bool wasPressed = m_ButtonsPressed[i];
		m_ButtonsPressed[i] = glfwGetMouseButton(m_Window, i);
		bool isPressed = m_ButtonsPressed[i];

		if (!wasPressed && isPressed)
		{
			m_ButtonsDown[i] = true;
		}
		else if (wasPressed && !isPressed)
		{
			m_ButtonsUp[i] = true;
		}
	}
}

void Mouse::HideCursor()
{
	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Mouse::ShowCursor()
{
	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool Mouse::ButtonPressed(MouseButton button)
{	
	return m_ButtonsPressed[(int)button];

}

bool Mouse::ButtonDown(MouseButton button)
{
	return m_ButtonsDown[(int)button];
}

bool Mouse::ButtonUp(MouseButton button)
{
	return m_ButtonsUp[(int)button];
}