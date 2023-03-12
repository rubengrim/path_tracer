#pragma once
#include "Eigen/Dense"

namespace BSDF::Lambertian {

	Eigen::Vector3f Eval(Eigen::Vector3f albedo, Eigen::Vector3f incidentDirection);
	float Pdf(Eigen::Vector3f incidentDirection);
	Eigen::Vector3f Sample(Eigen::Vector3f albedo, Eigen::Vector3f* incidentDirection);

}