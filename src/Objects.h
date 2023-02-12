#pragma once
#include "Eigen/Dense"
#include "Color.h"

struct Sphere
{
	Sphere() = default;
	Sphere(Eigen::Vector3f center, float radius, int materialID)
		: Center(center), Radius(radius), MaterialID(materialID)
	{
	}

	Eigen::Vector3f Center;
	float Radius;
	int MaterialID;
};