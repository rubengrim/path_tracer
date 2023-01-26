#pragma once
#include "Eigen/Dense"
#include "Logger.h"
#include <typeinfo>

namespace MathUtils{

	template<typename T>
	T ClampMax(T vec, float max)
	{
		for (int r = 0; r < vec.rows(); r++)
		{
			if (vec(r) > max)
				vec(r) = max;
		}

		return vec;
	}

	template<typename T>
	T ClampMin(T vec, float min)
	{
		for (int r = 0; r < vec.rows(); r++)
		{
			if (vec(r) < min)
				vec(r) = min;
		}

		return vec;
	}

	template<typename T>
	T Clamp(T vec, float min, float max)
	{
		for (int r = 0; r < vec.rows(); r++)
		{
			if (vec(r) < min)
				vec(r) = min;
			else if (vec(r) > max)
				vec(r) = max;
		}

		return vec;
	}

}