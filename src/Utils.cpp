#include "Utils.h"
#include "Random.h"

namespace Utils {

	template<typename T>
	T ClampVMin(T vec, float min)
	{
		for (int r = 0; r < vec.rows(); r++)
		{
			if (vec(r) < min)
				vec(r) = min;
		}

		return vec;
	}

	template<typename T>
	T ClampVMax(T vec, float max)
	{
		for (int r = 0; r < vec.rows(); r++)
		{
			if (vec(r) > max)
				vec(r) = max;
		}

		return vec;
	}

	template<typename T>
	T ClampV(T vec, float min, float max)
	{
		vec = ClampVecMin(vec, min);
		vec = ClampVecMax(vec, max);

		return vec;
	}

	float ClampfMin(float f, float min)
	{
		return (f > min) ? f : min;
	}

	float ClampfMax(float f, float max)
	{
		return (f < max) ? f : max;
	}

	float Clampf(float f, float min, float max)
	{
		f = ClampfMin(f, min);
		f = ClampfMax(f, max);
		return f;
	}

	Eigen::Matrix3f CreateOrthogonalBasis(Eigen::Vector3f n)
	{
		// Create orthogonal basis vectors from n.
		Eigen::Vector3f s = Eigen::Vector3f::Zero();
		while (s == Eigen::Vector3f::Zero())
			s = n.cross(Random::Vec3f());
		
		//s = WorldNormal.cross(Eigen::Vector3f(WorldNormal.z(), WorldNormal.x(), WorldNormal.y())).normalized();
		Eigen::Vector3f t = s.cross(n).normalized();

		// T is the local-to-world matrix, T's inverse is the world-to-local.
		Eigen::Matrix3f T;
		T << s.x(), t.x(), n.x(),
			s.y(), t.y(), n.y(),
			s.z(), t.z(), n.z();

		return T;
	}

}