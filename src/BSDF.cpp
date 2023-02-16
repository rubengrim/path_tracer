#include "BSDF.h"
#include "Utils.h"
#include "Random.h"

#include <cmath>

namespace BSDF::Lambertian {

	Color Eval(Color albedo, Eigen::Vector3f incidentDirection)
	{
		return albedo * INV_PI * incidentDirection.z();
	}

	float Pdf(Eigen::Vector3f incidentDirection)
	{
		// This is calculating cos(theta) * INV_PI, where theta is the angle between incident and normal, but since incident is in local coords cos(theta) is just its z-value.
		return incidentDirection.z() * INV_PI;
	}

	Color Sample(Eigen::Vector3f* incidentDirection, Color albedo)
	{
		float u1 = Random::Float();
		float u2 = Random::Float();
		
		float r = sqrt(u1);
		float theta = TWO_PI * u2;

		float x = r * cos(theta);
		float y = r * sin(theta);

		*incidentDirection = Eigen::Vector3f(x, y, std::max(0.0f, sqrt(1.0f - u1))).normalized();

		// Eval() / PDF() * cos(theta) = albedo
		return albedo;

		//float u1 = Random::Float();
		//float u2 = Random::Float();

		//float r = sqrt(1.0f - u1 * u1);
		//float phi = TWO_PI * u2;

		//context.LocalIncidentDirection = Eigen::Vector3f(cos(phi) * r, sin(phi) * r, u1).normalized();

		//return Eval(context) * (1.0f / Pdf(context));
	}

}

