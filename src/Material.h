#pragma once
#include "Color.h"
#include "BSDF.h"
#include "Scene.h"

#include <string>

struct Material
{
	Material() = default;
	
	Material(std::string name, Color albedo)
		: Name(name), Albedo(albedo), EmitsLight(false), LightIntensity(1.0f), LightColor(Color::White())
	{
	}

	Material(std::string name, Color albedo, bool emitsLight, float lightIntensity, Color lightColor)
		: Name(name), Albedo(albedo), EmitsLight(emitsLight), LightIntensity(lightIntensity), LightColor(lightColor)
	{
	}

	// Calls Eval/PDF/Sample functions of corresponding BSDF depending on material properties.
	//Color Eval(Eigen::Vector3f incidentDirection, Eigen::Vector3f outgoingDirection, Eigen::Vector3f normal)
	//{
	//	return BSDF::Lambertian::Eval(context);
	//}

	//float Pdf(HitContext& context)
	//{
	//	return BSDF::Lambertian::Pdf(context);
	//}

	//Color Sample(HitContext& context)
	//{
	//	return BSDF::Lambertian::Sample(context);
	//}

	std::string Name;
	Color Albedo;
	bool EmitsLight;
	float LightIntensity;
	Color LightColor;
};