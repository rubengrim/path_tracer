#include "Intersections.h"
#include "Logger.h"
#include <cmath>

bool Intersections::RaySphere(Ray ray, Sphere sphere, Eigen::Vector3f& atPos, float& atT)
{
	Eigen::Vector3f l = ray.Origin - sphere.Center;
	float a = ray.Direction.dot(ray.Direction);
	float b = 2 * ray.Direction.dot(l);
	float c = l.dot(l) - (sphere.Radius * sphere.Radius);

	float t;
	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0.0f) // No intersection
	{
		atPos = sphere.Center;
		atT = -1.0f;
		return false;
	}
	else if (discriminant == 0.0f) // One point of intersection
	{
		t = -b / (2 * a);
	}
	else if (discriminant > 0.0f) // Two points of intersection
	{
		float t1 = (-b + sqrt(discriminant)) / (2 * a);
		float t2 = (-b - sqrt(discriminant)) / (2 * a);
		t = std::min(t1, t2);
	}

	if (t < 0.0f)
	{
		atPos = sphere.Center;
		atT = -1.0f;
		return false;
	}

	atT = t;
	atPos = ray.Origin + t * ray.Direction;
	return true;

	//Eigen::Vector3f m = ray.Origin - sphere.Center;
	//float b = m.dot(ray.Direction);
	//float c = m.dot(m) - sphere.Radius * sphere.Radius;

	//// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0) 
	//if (c > 0.0f && b > 0.0f) return 0;
	//float discr = b * b - c;

	//// A negative discriminant corresponds to ray missing sphere 
	//if (discr < 0.0f) return 0;

	//// Ray now found to intersect sphere, compute smallest t value of intersection
	//float t = -b - sqrt(discr);

	//// If t is negative, ray started inside sphere so clamp t to zero 
	//if (t < 0.0f) t = 0.0f;

	//atT = t;
	//atPos = ray.Origin + t * ray.Direction;

	//return 1;
}