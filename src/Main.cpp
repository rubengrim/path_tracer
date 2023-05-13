#include "Application.h"

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
