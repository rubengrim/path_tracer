#pragma once
#include "Eigen/Dense"
#include "Camera.h"
#include "Ray.h"
#include "Image.h"
#include "Scene.h"
#include "Color.h"

#include <stdint.h>

struct Payload
{
	int ObjectIndex;
	float HitDistance;
	Eigen::Vector3f Position;
	Eigen::Vector3f Normal;
	Color Color;
};

class Renderer
{
public:
	bool Render(Image& image, Scene& scene, const Camera& camera, float& timeToRender);

private:

	Color PerPixel(int x, int y);
	void RenderBlock(int yBegin, int yEnd);
	Color TracePath(Ray ray);
	Payload CastRay(Ray ray);
	Payload HitShader(Ray ray, float hitDistance, int objectIndex);

private:
	Scene* m_Scene = nullptr;
	const Camera* m_Camera = nullptr;
	uint32_t* m_ImageData = nullptr;
};