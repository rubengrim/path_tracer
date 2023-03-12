#include "Utils.h"
#include "Random.h"

//template<typename T>
//T Utils::ClampVMin(T vec, float min)
//{
//	for (int r = 0; r < vec.rows(); r++)
//	{
//		if (vec(r) < min)
//			vec(r) = min;
//	}
//
//	return vec;
//}
//
//template<typename T>
//T Utils::ClampVMax(T vec, float max)
//{
//	for (int r = 0; r < vec.rows(); r++)
//	{
//		if (vec(r) > max)
//			vec(r) = max;
//	}
//
//	return vec;
//}
//
//template<typename T>
//T Utils::ClampV(T vec, float min, float max)
//{
//	vec = ClampVMin(vec, min);
//	vec = ClampVMax(vec, max);
//
//	return vec;
//}

float Utils::ClampfMin(float f, float min)
{
	return (f > min) ? f : min;
}

float Utils::ClampfMax(float f, float max)
{
	return (f < max) ? f : max;
}

float Utils::Clampf(float f, float min, float max)
{
	f = ClampfMin(f, min);
	f = ClampfMax(f, max);
	return f;
}

Eigen::Matrix3f Utils::CreateOrthogonalBasis(Eigen::Vector3f n)
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

uint32_t Utils::Color::FloatToRGBA(Eigen::Vector3f floatColor)
{
	uint8_t r = (uint8_t)(floatColor.x() * 255.0f);
	uint8_t g = (uint8_t)(floatColor.y() * 255.0f);
	uint8_t b = (uint8_t)(floatColor.z() * 255.0f);
	uint8_t a = (uint8_t)(255.0f);

	return (a << 24) | (b << 16) | (g << 8) | r;
}
