#include "Color.h"

Color::Color(float r, float g, float b, float a)
	: R(r), G(g), B(b), A(a)
{
}

Color::Color(float r, float g, float b)
	: R(r), G(g), B(b), A(1.0f)
{
}

uint32_t Color::RGBA()
{
	Clamp();
	return CalculateRGBA();
}

uint32_t Color::CalculateRGBA()
{
	uint8_t r = (uint8_t)(this->R * 255.0f);
	uint8_t g = (uint8_t)(this->G * 255.0f);
	uint8_t b = (uint8_t)(this->B * 255.0f);
	uint8_t a = (uint8_t)(this->A * 255.0f);

	return (a << 24) | (b << 16) | (g << 8) | r;
}

void Color::Clamp()
{
	Clamp(0.0f, 1.0f);
}

void Color::Clamp(float min, float max)
{
	this->R = (this->R > min) ? this->R : min;
	this->R = (this->R < max) ? this->R : max;

	this->G = (this->G > min) ? this->G : min;
	this->G = (this->G < max) ? this->G : max;

	this->B = (this->B > min) ? this->B : min;
	this->B = (this->B < max) ? this->B : max;

	this->A = (this->A > min) ? this->A : min;
	this->A = (this->A < max) ? this->A : max;
}

Color Color::operator+(const Color& other) const
{
	Color ret;
	ret.R = this->R + other.R;
	ret.G = this->G + other.G;
	ret.B = this->B + other.B;
	ret.A = this->A + other.A;
	return ret;
}

Color Color::operator-(const Color& other) const
{
	Color ret;
	ret.R = this->R - other.R;
	ret.G = this->G - other.G;
	ret.B = this->B - other.B;
	ret.A = this->A - other.A;
	return ret;
}

Color Color::operator*(const float& factor) const
{
	Color ret;
	ret.R = this->R * factor;
	ret.G = this->G * factor;
	ret.B = this->B * factor;
	return ret;
}

Color& Color::operator+=(const Color& other)
{
	this->R += other.R;
	this->G += other.G;
	this->B += other.B;
	this->A += other.A;
	return *this;
}

Color& Color::operator-=(const Color& other)
{
	this->R -= other.R;
	this->G -= other.G;
	this->B -= other.B;
	this->A -= other.A;
	return *this;
}

Color& Color::operator*=(const float& factor)
{
	this->R *= factor;
	this->G *= factor;
	this->B *= factor;
	return *this;
}

Color operator*(const float& factor, const Color& color)
{
	return color * factor;
}