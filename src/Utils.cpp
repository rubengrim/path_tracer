#include "Utils.h"

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

}