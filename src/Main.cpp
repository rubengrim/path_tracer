#include "Application.h"
#include <stdint.h>
#include <iostream>

// Entry point
int main()
{
	//Eigen::AngleAxisf yawAngle(0.125f * 3.1415f, Eigen::Vector3f::UnitY());
	//Eigen::AngleAxisf pitchAngle(0.125f * 3.1415f, Eigen::Vector3f::UnitX());

	//Eigen::Quaternion<float> q = yawAngle * pitchAngle;

	//Eigen::Matrix3f rotationMatrix = q.matrix();

	//

	//Eigen::Matrix4f transformation;
	//transformation.setIdentity();
	//transformation.block<3, 3>(0, 0) = rotationMatrix;
	//transformation.block<3, 1>(0, 3) = Eigen::Vector3f(6.0f, 7.0f, 8.0f);

	//for (int row = 0; row < 4; row++)
	//{
	//	std::cout << std::endl;
	//	for (int col = 0; col < 4; col++)
	//	{
	//		std::cout << transformation(row, col) << "  ";
	//	}
	//}


#if 1
	Application* application = new Application();
	if (application->Initialize())
	{
		application->Run();
	}
	delete application;
#endif

	return 0;
}