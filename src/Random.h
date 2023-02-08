#pragma once
#include <random>
#include <stdint.h>

#include "Eigen/Dense"

class Random
{
public:

	static void Initialize()
	{
		s_Generator.seed(std::random_device()());
	}

	static uint32_t UInt()
	{
		return s_Distribution(s_Generator);
	}

	static uint32_t UInt(uint32_t min, uint32_t max)
	{
		return min + (s_Distribution(s_Generator) % (max - min + 1));
	}

	static float Float()
	{
		return (float)s_Distribution(s_Generator) / (float)std::numeric_limits<int>::max();
	}

	static Eigen::Vector2f Vec2f()
	{
		return Eigen::Vector2f(Float(), Float());
	}

	static Eigen::Vector3f Vec3f()
	{
		return Eigen::Vector3f(Float(), Float(), Float());
	}

private:
	inline static thread_local std::mt19937 s_Generator;
	inline static std::uniform_int_distribution<int> s_Distribution;
};