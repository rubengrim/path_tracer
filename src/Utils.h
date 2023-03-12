#pragma once
#include "Eigen/Dense"
#include <typeinfo>

constexpr float PI = 3.1415926;
constexpr float TWO_PI = 6.2831853;
constexpr float INV_PI = 0.3183098;

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
		vec = ClampVMin(vec, min);
		vec = ClampVMax(vec, max);

		return vec;
	}

	float ClampfMin(float f, float min);

	float ClampfMax(float f, float max);

	float Clampf(float f, float min, float max);

	Eigen::Matrix3f CreateOrthogonalBasis(Eigen::Vector3f n);

}

namespace Utils::Color {

	inline Eigen::Vector3f White() { return Eigen::Vector3f(1.0f, 1.0f, 1.0f); }
	inline Eigen::Vector3f LightGrey() { return Eigen::Vector3f(0.5f, 0.5f, 0.5f); }
	inline Eigen::Vector3f DarkGrey() { return Eigen::Vector3f(0.1f, 0.1f, 0.1f); }
	inline Eigen::Vector3f Black() { return Eigen::Vector3f(0.0f, 0.0f, 0.0f); }
	inline Eigen::Vector3f Red() { return Eigen::Vector3f(1.0f, 0.0f, 0.0f); }
	inline Eigen::Vector3f Green() { return Eigen::Vector3f(0.0f, 1.0f, 0.0f); }
	inline Eigen::Vector3f Blue() { return Eigen::Vector3f(0.0f, 0.0f, 1.0f); }

	uint32_t FloatToRGBA(Eigen::Vector3f floatColor);
}


