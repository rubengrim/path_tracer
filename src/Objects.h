#pragma once
#include "Eigen/Dense"
#include "Color.h"
#include "Random.h"

struct Sphere
{
	Sphere() = default;
	Sphere(Eigen::Vector3f center, float radius, int materialID)
		: Center(center), Radius(radius), MaterialID(materialID)
	{
	}

	Eigen::Vector3f SamplePointOnSurface()
	{
		Eigen::Vector3f dir = Random::Vec3f().normalized();
		return Center + dir * Radius;
	}

	Eigen::Vector3f Center;
	float Radius;
	int MaterialID;
};