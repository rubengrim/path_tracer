#include "BSDF.h"
#include "Utils.h"
#include "Random.h"

#include <cmath>

namespace BSDF::Lambertian {

	Color Eval(HitContext& context)
	{
		return context.Albedo * INV_PI;
	}

	float Pdf(HitContext& context)
	{
		return context.LocalIncidentDirection.z() * INV_PI;
	}

	Color Sample(HitContext& context)
	{
		float u1 = Random::Float();
		float u2 = Random::Float();
		
		float r = sqrt(u1);
		float theta = TWO_PI * u2;

		float x = r * cos(theta);
		float y = r * sin(theta);

		context.LocalIncidentDirection = Eigen::Vector3f(x, y, std::max(0.0f, sqrt(1.0f - u1))).normalized();

		// Eval() / PDF() * cos(theta) = albedo
		return context.Albedo;

		//float u1 = Random::Float();
		//float u2 = Random::Float();

		//float r = sqrt(1.0f - u1 * u1);
		//float phi = TWO_PI * u2;

		//context.LocalIncidentDirection = Eigen::Vector3f(cos(phi) * r, sin(phi) * r, u1).normalized();

		//return Eval(context) * (1.0f / Pdf(context));
	}

}

