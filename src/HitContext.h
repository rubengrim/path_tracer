#pragma once
#include "Eigen/Dense"
#include "Color.h"
#include "Random.h"

class HitContext
{
public:
	HitContext() = default;

	HitContext(Eigen::Vector3f worldNormal)
	{
		CorrespondsToValidHit = true;

		WorldNormal = worldNormal;

		// Create orthogonal basis vectors from n.
		s = Eigen::Vector3f::Zero();
		while (s == Eigen::Vector3f::Zero())
		{
			s = WorldNormal.cross(Random::Vec3f());
		}
		//s = WorldNormal.cross(Eigen::Vector3f(WorldNormal.z(), WorldNormal.x(), WorldNormal.y())).normalized();
		t = s.cross(WorldNormal).normalized();

		Eigen::Matrix3f T;
		T << s.x(), t.x(), WorldNormal.x(),
			 s.y(), t.y(), WorldNormal.y(),
			 s.z(), t.z(), WorldNormal.z();

		m_ToWorldMatrix = T;
		m_ToLocalMatrix = T.inverse();
	}

	Eigen::Vector3f ToLocal(Eigen::Vector3f vec) { return m_ToLocalMatrix * vec; }
	Eigen::Vector3f ToWorld(Eigen::Vector3f vec) { return m_ToWorldMatrix * vec; }

	bool CorrespondsToValidHit;

	int MaterialIndex;
	Color Albedo;
	
	float HitDistance;

	Eigen::Vector3f s;
	Eigen::Vector3f t;

	Eigen::Vector3f WorldPosition;
	Eigen::Vector3f WorldNormal;

	Eigen::Vector3f LocalOutgoingDirection;
	Eigen::Vector3f LocalIncidentDirection;
	
private:
	Eigen::Matrix3f m_ToLocalMatrix;
	Eigen::Matrix3f m_ToWorldMatrix;
};