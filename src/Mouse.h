#pragma once
#include <array>
#include "GLFW/glfw3.h"

typedef enum class MouseButton;

class Mouse
{
public:
	static void Initialize(GLFWwindow* window);
	static void Update();

	// Get x and y position of mouse.
	inline static double GetX() { return m_X; }
	inline static double GetY() { return m_Y; }

	// Get x and y position of mouse last frame.
	inline static double GetLastX() { return m_XLast; }
	inline static double GetLastY() { return m_YLast; }

	// Get the change in x and y position of mouse since last frame.
	inline static double GetDeltaX() { return m_X - m_XLast; }
	inline static double GetDeltaY() { return m_Y - m_YLast; }

	static void HideCursor();
	static void ShowCursor();

	static bool ButtonPressed(MouseButton button);
	static bool ButtonDown(MouseButton button);
	static bool ButtonUp(MouseButton button);

private:
	static GLFWwindow* m_Window;
		
	constexpr static const int m_ButtonCount = 8;

	static double m_X;
	static double m_Y;
	static double m_XLast;
	static double m_YLast;

	static std::array<bool, m_ButtonCount> m_ButtonsPressed;
	static std::array<bool, m_ButtonCount> m_ButtonsDown;
	static std::array<bool, m_ButtonCount> m_ButtonsUp;
};

typedef enum class MouseButton
{
	// Taken from GLFW - see "glfw3.h"
	Button1 = 0,
	Button2 = 1,
	Button3 = 2,
	Button4 = 3,
	Button5 = 4,
	Button6 = 5,
	Button7 = 6,
	Button8 = 7,
	Last = Button8,
	Left = Button1,
	Right = Button2,
	Middle = Button3
};