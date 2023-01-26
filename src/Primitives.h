#pragma once
#include "Eigen/Dense"
#include "Color.h"

struct Sphere
{
	Sphere() = default;
	Sphere(Eigen::Vector3f center, float radius, Color albedo)
		: Center(center), Radius(radius), Albedo(albedo)
	{
	}

	Eigen::Vector3f Center;
	float Radius;
	Color Albedo;
};