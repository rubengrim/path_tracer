#include "Color.h"

Color::Color(float r, float g, float b, float a)
	: R(r), G(g), B(b), A(a)
{
}

Color::Color(float r, float g, float b)
	: R(r), G(g), B(b), A(1.0f)
{
}

Color::Color(Eigen::Vector3f vec3)
	: R(vec3.x()), G(vec3.y()), B(vec3.z()), A(1.0f)
{
}

Eigen::Vector3f Color::ToVec3()
{
	return Eigen::Vector3f(R, G, B);
}

float* Color::GetData3()
{
	float data[3] = { R, G, B };
	return data;
}

float* Color::GetData4()
{
	float data[4] = { R, G, B, A };
	return data;
}

void Color::SetData3(float data[3])
{
	R = data[0];
	G = data[1];
	B = data[2];
}

void Color::SetData4(float data[4])
{
	R = data[0];
	G = data[1];
	B = data[2];
	A = data[3];
}

uint32_t Color::RGBA()
{
	Clamp();
	return CalculateRGBA();
}

uint32_t Color::CalculateRGBA()
{
	uint8_t r = (uint8_t)(R * 255.0f);
	uint8_t g = (uint8_t)(G * 255.0f);
	uint8_t b = (uint8_t)(B * 255.0f);
	uint8_t a = (uint8_t)(A * 255.0f);

	return (a << 24) | (b << 16) | (g << 8) | r;
}

void Color::Clamp()
{
	Clamp(0.0f, 1.0f);
}

void Color::Clamp(float min, float max)
{
	R = (R > min) ? R : min;
	R = (R < max) ? R : max;

	G = (G > min) ? G : min;
	G = (G < max) ? G : max;

	B = (B > min) ? B : min;
	B = (B < max) ? B : max;

	A = (A > min) ? A : min;
	A = (A < max) ? A : max;
}

Color Color::operator+(const Color& other) const
{
	Color ret;
	ret.R = R + other.R;
	ret.G = G + other.G;
	ret.B = B + other.B;
	ret.A = A;
	return ret;
}

Color Color::operator-(const Color& other) const
{
	Color ret;
	ret.R = R - other.R;
	ret.G = G - other.G;
	ret.B = B - other.B;
	ret.A = A;
	return ret;
}

Color Color::operator*(const Color& other) const
{
	Color ret;
	ret.R = R * other.R;
	ret.G = G * other.G;
	ret.B = B * other.B;
	ret.A = A;
	return ret;
}

Color Color::operator*(const float& factor) const
{
	Color ret;
	ret.R = R * factor;
	ret.G = G * factor;
	ret.B = B * factor;
	ret.A = A;
	return ret;
}

Color& Color::operator+=(const Color& other)
{
	R += other.R;
	G += other.G;
	B += other.B;
	return *this;
}

Color& Color::operator-=(const Color& other)
{
	R -= other.R;
	G -= other.G;
	B -= other.B;
	return *this;
}

Color& Color::operator*=(const Color& other)
{
	R *= other.R;
	G *= other.G;
	B *= other.B;
	return *this;
}

Color& Color::operator*=(const float& factor)
{
	R *= factor;
	G *= factor;
	B *= factor;
	return *this;
}

Color operator*(const float& factor, const Color& color)
{
	return color * factor;
}