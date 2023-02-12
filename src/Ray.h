#pragma once
#include "Eigen/Dense"

struct RayPayload
{
	bool RayHitSomething;
	int ObjectID;
	float HitDistance;
	Eigen::Vector3f WorldPosition;
	Eigen::Vector3f WorldNormal;
};

struct Ray
{
	Eigen::Vector3f Origin;
	Eigen::Vector3f Direction;
	RayPayload Payload;

	inline Eigen::Vector3f At(float t) { return Origin + t * Direction; }
};