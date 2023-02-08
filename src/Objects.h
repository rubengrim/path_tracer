#pragma once
#include "Eigen/Dense"
#include "Color.h"

struct Sphere
{
	Sphere() = default;
	Sphere(Eigen::Vector3f center, float radius, int materialIndex)
		: Center(center), Radius(radius), MaterialIndex(materialIndex)
	{
	}

	Eigen::Vector3f Center;
	float Radius;
	int MaterialIndex;
};