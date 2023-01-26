#pragma once
#include "Eigen/Dense"
#include "Camera.h"
#include "Ray.h"
#include "Image.h"
#include "Scene.h"
#include "Color.h"

#include <stdint.h>

class Renderer
{
public:
	static bool Render(Image* image, Scene* scene, Camera* camera, float& timeToRender);
	static Color TraceRay(Ray ray, Scene* scene);
	static uint32_t FloatToRGBA(Eigen::Vector4f floatColor);
};