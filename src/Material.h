#pragma once
#include "Color.h"
#include "BSDF.h"
#include "Scene.h"

class Material
{
public:
	Material() = default;
	Material(Color albedo)
		: Albedo(albedo)
	{
	}

	// Calls Eval/PDF/Sample functions of corresponding BSDF depending on material properties.
	Color Eval(HitContext& context)
	{
		return BSDF::Lambertian::Eval(context);
	}

	float Pdf(HitContext& context)
	{
		return BSDF::Lambertian::Pdf(context);
	}

	Color Sample(HitContext& context)
	{
		return BSDF::Lambertian::Sample(context);
	}
	
	Color Albedo;
};