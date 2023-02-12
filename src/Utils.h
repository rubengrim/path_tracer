#pragma once
#include "Eigen/Dense"
#include "Logger.h"
#include <typeinfo>

constexpr float PI = 3.1415926;
constexpr float TWO_PI = 6.2831853;
constexpr float INV_PI = 0.3183098;

namespace Utils {

	template<typename T>
	T ClampVMin(T vec, float min);

	template<typename T>
	T ClampVMax(T vec, float max);

	template<typename T>
	T ClampV(T vec, float min, float max);

	float ClampfMin(float f, float min);

	float ClampfMax(float f, float max);

	float Clampf(float f, float min, float max);

	Eigen::Matrix3f CreateOrthogonalBasis(Eigen::Vector3f n);

}


