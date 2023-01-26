#pragma once
#include "Eigen/Dense"
#include "MathUtils.h"

#include <stdint.h>

class Color
{
public:
	static Color Red() { return Color(1.0f, 0.0f, 0.0f); }
	static Color Green() { return Color(0.0f, 1.0f, 0.0f); }
	static Color Blue() { return Color(0.0f, 0.0f, 1.0f); }
	static Color Black() { return Color(0.0f, 0.0f, 0.0f); }
	static Color White() { return Color(1.0f, 1.0f, 1.0f); }

public:
	Color() = default;
	Color(float r, float g, float b);
	Color(float r, float g, float b, float a);
	
	uint32_t RGBA();
	
	void Clamp();
	void Clamp(float min, float max);

	Color operator+(const Color& other) const;
	Color operator-(const Color& other) const;
	Color operator*(const float& other) const;

	Color& operator+=(const Color& other);
	Color& operator-=(const Color& other);
	Color& operator*=(const float& other);

	float R;
	float G;
	float B;
	float A;

private:
	uint32_t CalculateRGBA();
};

static Color operator*(const float& factor, const Color& color);