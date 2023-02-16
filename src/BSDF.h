#pragma once
#include "Color.h"
#include "Eigen/Dense"

namespace BSDF::Lambertian {

	Color Eval(Color albedo, Eigen::Vector3f incidentDirection);
	float Pdf(Eigen::Vector3f incidentDirection);
	Color Sample(Eigen::Vector3f* incidentDirection, Color albedo);

}