#pragma once
#include "Eigen/Dense"

struct Ray
{
	Eigen::Vector3f Origin;
	Eigen::Vector3f Direction;

	inline Eigen::Vector3f At(float t) { return Origin + t * Direction; }
};