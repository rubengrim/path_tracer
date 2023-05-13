#pragma once
#include "Eigen/Dense"
#include "Random.h"
#include "Utils.h"
#include <cmath>

class Sphere
{
public:
	Sphere() = default;
	Sphere(Eigen::Vector3f center, float radius, int materialID)
		: Center(center), Radius(radius), MaterialID(materialID)
	{
	}

  // Uniformly samples a point on the surface according to a probability density with respect to the solid angle from a reference point. 
	float SamplePointOnSurface(Eigen::Vector3f refPoint, Eigen::Vector3f* sampledPoint)
	{
    float theta = 2.0f * PI * Random::Float();
    float phi = std::acos(2.0f * Random::Float() - 1.0f); 
    
    // Coordinates for a random uniformly distributed point on the unit sphere.
    float x = std::cos(theta) * std::sin(phi);
    float y = std::sin(theta) * std::sin(phi);
    float z = std::cos(phi);

    *sampledPoint = Center + Radius * Eigen::Vector3f(x, y, z); 
    
    Eigen::Vector3f surfaceNormal = *sampledPoint - Center;
    float cosTerm = surfaceNormal.normalized().dot((refPoint - *sampledPoint).normalized());
    if (cosTerm <= 0.0f)
    {
      return 0.0f; // The PDF is 0 if the sampled point is not seen from the reference point.
    }
    else
    {
      float inverseArea = 4.0f * PI * Radius * Radius;
      return inverseArea * (refPoint - *sampledPoint).squaredNorm() / cosTerm; 
    }

	}

public:
	Eigen::Vector3f Center;
	float Radius;
	int MaterialID;
};
