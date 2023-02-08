#pragma once
#include "Color.h"
#include "Eigen/Dense"
#include "HitContext.h"

namespace BSDF::Lambertian {

	Color Eval(HitContext& context);
	float Pdf(HitContext& context);
	Color Sample(HitContext& context);

}