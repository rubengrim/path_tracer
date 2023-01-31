#include "Application.h"
#include <stdint.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "Camera.h"

#include "glad/gl.h"
#include "GLFW/glfw3.h"

// Entry point
int main()
{

#define RUN 1
#if RUN
	Application* application = new Application();
	if (application->Initialize())
	{
		application->Run();
	}
	delete application;
#endif

	return 0;
}