#pragma once
#include "Eigen/Dense"
#include "Camera.h"
#include "Ray.h"
#include "Image.h"
#include "Scene.h"
#include "Color.h"
#include "Material.h"
#include "HitContext.h"

#include <stdint.h>

class Renderer
{
public:
	Renderer(int viewportHeight, int viewportWidth);
	bool Render(Image& image, Scene& scene, const Camera& camera, float& timeToRender);
	void Resize(int width, int height);
	inline void ResetAccumulation() { m_FrameIndex = 1; }

private:

	Color PerPixel(int x, int y);
	HitContext CastRay(Ray ray);

private:
	int m_ViewportWidth = 0;
	int m_ViewportHeight = 0;

	Scene* m_Scene = nullptr;
	const Camera* m_Camera = nullptr;

	uint32_t* m_ImageData = nullptr;
	Eigen::Vector3f* m_AccumulatedData = nullptr;
	int m_FrameIndex = 1;
};