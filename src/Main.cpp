#include "Application.h"
#include <stdint.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "Camera.h"
#include "Eigen/Dense"

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "Random.h"
#include "BSDF.h"
#include "Color.h"
#include "Material.h"


// Entry point
int main()
{
	//Color c1(0.5f, 0.5f, 0.5f);
	//Color c2 = 100.0f * c1;
	//std::cout << c2.R << "  " << c2.G << "  " << c2.B << std::endl;


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