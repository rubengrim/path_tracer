#include "Keyboard.h"

#include "GLFW/glfw3.h"

GLFWwindow* Keyboard::m_Window = nullptr;

std::array<bool, Keyboard::m_KeyCount> Keyboard::m_KeysPressed;
std::array<bool, Keyboard::m_KeyCount> Keyboard::m_KeysDown;
std::array<bool, Keyboard::m_KeyCount> Keyboard::m_KeysUp;

void Keyboard::Initialize(GLFWwindow* window)
{
	m_Window = window;

	std::fill(m_KeysPressed.begin(), m_KeysPressed.end(), false);
	std::fill(m_KeysDown.begin(), m_KeysDown.end(), false);
	std::fill(m_KeysUp.begin(), m_KeysUp.end(), false);
}

void Keyboard::Update()
{
	std::fill(m_KeysDown.begin(), m_KeysDown.end(), false);
	std::fill(m_KeysUp.begin(), m_KeysUp.end(), false);

	for (int i = (int)Key::First; i < m_KeyCount; i++)
	{
		bool wasPressed = m_KeysPressed[i];
		m_KeysPressed[i] = glfwGetKey(m_Window, i);
		bool isPressed = m_KeysPressed[i];

		if (!wasPressed && isPressed)
		{
			m_KeysDown[i] = true;
		}
		else if (wasPressed && !isPressed)
		{
			m_KeysUp[i] = true;
		}
	}
}

bool Keyboard::KeyPressed(Key key)
{
	return m_KeysPressed[(int)key];
}

bool Keyboard::KeyDown(Key key)
{
	return m_KeysDown[(int)key];
}

bool Keyboard::KeyUp(Key key)
{
	return m_KeysUp[(int)key];
}