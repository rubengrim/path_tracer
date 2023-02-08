#pragma once
#include "Objects.h"
#include "Ray.h"
#include "Eigen/Dense"

namespace Intersections {

	bool RaySphere(Ray ray, Sphere sphere, Eigen::Vector3f& atPos, float& atT);

}