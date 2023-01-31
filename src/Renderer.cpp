#include "Renderer.h"
#include "Logger.h"
#include "Primitives.h"
#include "Intersections.h"
#include "MathUtils.h"

#include <cmath>
#include <chrono>
#include <thread>
#include <execution>
#include <algorithm>
#include <vector>

bool Renderer::Render(Image& image, Scene& scene, const Camera& camera, float& timeToRender)
{
	m_Scene = &scene;
	m_Camera = &camera;

	if (image.GetWidth() != m_Camera->GetViewportWidth() || image.GetHeight() != m_Camera->GetViewportHeight())
	{
		PULSE_ERROR("Trying to render to an image that has different dimensions than the camera.")
		return false;
	}

	m_ImageData = new uint32_t[m_Camera->GetViewportWidth() * m_Camera->GetViewportHeight()];

	auto start = std::chrono::high_resolution_clock::now();

#define MULTI_THREADING 1
#if MULTI_THREADING

	std::vector<int> yValues;
	for (int y = 0; y < m_Camera->GetViewportHeight(); y++)
		yValues.push_back(y);

	std::for_each(std::execution::par, yValues.begin(), yValues.end(), [&](int& y)
	{
		RenderBlock(y, y);
	});

#else

	RenderBlock(0, m_Camera->GetViewportHeight() - 1);

#endif

	auto finished = std::chrono::high_resolution_clock::now();
	timeToRender = std::chrono::duration_cast<std::chrono::microseconds>(finished - start).count() * 0.001f;

	image.SetImageData(m_ImageData);
	delete[] m_ImageData;

	return true;
}

void Renderer::RenderBlock(int yBegin, int yEnd)
{
	for (int y = yBegin; y <= yEnd; y++)
	{
		for (int x = 0; x < m_Camera->GetViewportWidth(); x++)
		{
			Color pixelColor = PerPixel(x, y);
			pixelColor.Clamp();
			m_ImageData[x + y * m_Camera->GetViewportWidth()] = pixelColor.RGBA();
		}
	}
}

Color Renderer::PerPixel(int x, int y)
{
	Ray ray;
	ray.Origin = m_Camera->GetPosition();
	ray.Direction = m_Camera->GetRayDirections()[x + y * m_Camera->GetViewportWidth()];

	Color pixelColor = Color::Black();

	int bounces = 5;

	float rayContributionFactor = 1.0f;
	for (int bounce = 0; bounce < bounces; bounce++)
	{
		Payload payload = CastRay(ray);
		
		if (payload.HitDistance == -1.0f)
		{
			pixelColor += Color::DarkGrey() * rayContributionFactor;
			break;
		}
		else
		{
			pixelColor += payload.Color * rayContributionFactor;
		
			ray.Origin = payload.Position + 0.0001f * payload.Normal;
			ray.Direction = (ray.Direction - 2 * ray.Direction.dot(payload.Normal) * payload.Normal).normalized();
		}

		rayContributionFactor *= 0.7f;
	}

	return pixelColor;
}

Color Renderer::TracePath(Ray ray)
{
	Color pixelColor = Color::Black();

	int bounces = 3;
	
	float multiplier = 1.0f;
	for (int bounce = 0; bounce < bounces; bounce++)
	{
		Payload payload = CastRay(ray);
		if (payload.HitDistance == -1.0f)
			pixelColor += Color::DarkGrey() * multiplier;

		pixelColor = payload.Color * multiplier;
		
		multiplier *= 0.7f;

		ray.Origin = payload.Position + 0.0001f * payload.Normal;
		ray.Direction = (ray.Direction - 2 * ray.Direction.dot(payload.Normal) * payload.Normal).normalized();
	}

	return pixelColor;
}

Payload Renderer::CastRay(Ray ray)
{
	int closestSphereIndex;
	float smallestT = -1.0f;
	Eigen::Vector3f intersectingAt;
	for (int i = 0; i < m_Scene->Spheres().size(); i++)
	{
		Eigen::Vector3f atPos;
		float atT;
		if (Intersections::RaySphere(ray, m_Scene->Spheres()[i], atPos, atT))
		{
			if (atT <= smallestT || smallestT == -1.0f)
			{
				closestSphereIndex = i;
				smallestT = atT;
				intersectingAt = atPos;
			}
		}
	}

	// No intersection found.
	if (smallestT == -1.0f)
	{
		Payload payload;
		payload.HitDistance = -1.0f;
		return payload;
	}
	
	return HitShader(ray, smallestT, closestSphereIndex);
}

Payload Renderer::HitShader(Ray ray, float hitDistance, int objectIndex)
{
	Payload payload;
	payload.ObjectIndex = objectIndex;
	payload.HitDistance = hitDistance;
	payload.Position = ray.At(payload.HitDistance);

	const Sphere& sphere = m_Scene->Spheres()[payload.ObjectIndex];
	payload.Normal = (payload.Position - sphere.Center).normalized();
	
	Eigen::Vector3f lightPosition(1.0f, 1.0f, -2.5f);
	Eigen::Vector3f toLight = (lightPosition - payload.Position).normalized();

	float cosineFactor = payload.Normal.dot(toLight);

	if (cosineFactor < 0.0f)
		cosineFactor = 0.0f;
	else if (cosineFactor > 1.0f)
		cosineFactor = 1.0f;

	payload.Color = sphere.Albedo * cosineFactor;

	return payload;
}