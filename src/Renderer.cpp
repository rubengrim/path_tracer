#include "Renderer.h"
#include "Logger.h"
#include "Primitives.h"
#include "Intersections.h"
#include "MathUtils.h"

#include <cmath>
#include <chrono>

bool Renderer::Render(Image* image, Scene* scene, Camera* camera, float& timeToRender)
{
	if (image->GetWidth() != camera->GetViewportWidth() || image->GetHeight() != camera->GetViewportHeight())
	{
		PULSE_ERROR("Trying to render to an image that has different dimensions than the camera.")
		return false;
	}

	uint32_t* imageData = new uint32_t[image->GetWidth() * image->GetHeight()];

	auto start = std::chrono::high_resolution_clock::now();
#if 1
	for (int y = 0; y < camera->GetViewportHeight(); y++)
		for (int x = 0; x < camera->GetViewportWidth(); x++)
		{
			Ray ray;
			ray.Origin = camera->GetPosition();
			ray.Direction = camera->GetRayDirections()[x + y * camera->GetViewportWidth()];
			Color pixelColor = TraceRay(ray, scene);
			imageData[x + y * camera->GetViewportWidth()] = pixelColor.RGBA();
		}
#endif
	auto finished = std::chrono::high_resolution_clock::now();
	timeToRender = std::chrono::duration_cast<std::chrono::microseconds>(finished - start).count() * 0.001f;

	image->SetImageData(imageData);
	delete[] imageData;

	return true;
}

Color Renderer::TraceRay(Ray ray, Scene* scene)
{
	// Find the closest intersecting sphere.
	int closestSphereIndex;
	float smallestT = -1.0f;
	Eigen::Vector3f intersectingAt;
	for (int i = 0; i < scene->GetSpheres().size(); i++)
	{
		Eigen::Vector3f atPos;
		float atT;
		if (Intersections::RaySphere(ray, scene->GetSpheres()[i], atPos, atT))
		{
			if (atT <= smallestT || smallestT == -1.0f)
			{
				closestSphereIndex = i;
				smallestT = atT;
				intersectingAt = atPos;
			}
		}
	}

	// The ray intersects no spheres.
	if (smallestT == -1.0f)
	{
		return Color::Blue();
	}

	const Sphere& closestSphere = scene->GetSpheres()[closestSphereIndex];

	Eigen::Vector3f normal = (intersectingAt - closestSphere.Center).normalized();
	Eigen::Vector3f lightPosition(1.0f, 1.0f, -2.5f);
	Eigen::Vector3f toLight = (lightPosition - intersectingAt).normalized();

	float cosineFactor = normal.dot(toLight);

	if (cosineFactor < 0.0f)
		cosineFactor = 0.0f;
	else if (cosineFactor > 1.0f)
		cosineFactor = 1.0f;

	Color pixelColor = closestSphere.Albedo;
	pixelColor *= cosineFactor;

	return pixelColor;
}

uint32_t Renderer::FloatToRGBA(Eigen::Vector4f floatColor)
{
	floatColor = MathUtils::Clamp(floatColor, 0.0f, 1.0f);

	uint8_t r = (uint8_t) (floatColor.x() * 255.0f);
	uint8_t g = (uint8_t) (floatColor.y() * 255.0f);
	uint8_t b = (uint8_t) (floatColor.z() * 255.0f);
	uint8_t a = (uint8_t) (floatColor.w() * 255.0f);

	return (a << 24) | (b << 16) | (g << 8) | r;
}