#include "Renderer.h"
#include "Logger.h"
#include "Objects.h"
#include "Intersections.h"
#include "Utils.h"

#include <cmath>
#include <chrono>
#include <thread>
#include <execution>
#include <algorithm>
#include <vector>

Renderer::Renderer(int viewportWidth, int viewportHeight)
	: m_ViewportWidth(viewportWidth), m_ViewportHeight(viewportHeight)
{
	m_ImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];

	m_AccumulatedData = new Eigen::Vector3f[m_ViewportWidth * m_ViewportHeight];
}

void Renderer::Resize(int viewportWidth, int viewportHeight)
{
	m_ViewportWidth = viewportWidth;
	m_ViewportHeight = viewportHeight;

	delete[] m_ImageData;
	m_ImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];

	delete[] m_AccumulatedData;
	m_AccumulatedData = new Eigen::Vector3f[m_ViewportWidth * m_ViewportHeight];
	
	m_FrameIndex = 1;
}

bool Renderer::Render(Image& image, Scene& scene, const Camera& camera, float& timeToRender)
{
	auto start = std::chrono::high_resolution_clock::now();

	m_Scene = &scene;
	m_Camera = &camera;

	if (m_FrameIndex == 1)
		memset(m_AccumulatedData, 0, m_ViewportWidth * m_ViewportHeight * sizeof(Eigen::Vector3f));

#define MULTI_THREADING 1
#if MULTI_THREADING

	std::vector<int> yValues;
	for (int y = 0; y < m_ViewportHeight; y++)
		yValues.push_back(y);

	std::for_each(std::execution::par, yValues.begin(), yValues.end(), [&](int& y)
	{
		for (int x = 0; x < m_ViewportWidth; x++)
		{
			Color pixelColor = PerPixel(x, y);
			pixelColor.Clamp();
			m_AccumulatedData[x + y * m_ViewportWidth] += pixelColor.ToVec3();

			Color accumulatedPixelColor(m_AccumulatedData[x + y * m_ViewportWidth] * (1.0f / m_FrameIndex));
			m_ImageData[x + y * m_ViewportWidth] = accumulatedPixelColor.RGBA();
		}
	});

#else

	for (int y = 0; y < m_ViewportHeight; y++)
	{
		for (int x = 0; x < m_ViewportWidth; x++)
		{
			//Color pixelColor = PerPixel(x, y);
			//pixelColor.Clamp();
			//m_AccumulatedData[x + y * m_ViewportWidth] += pixelColor.ToVec3();

			//Color accumulatedPixelColor(m_AccumulatedData[x + y * m_ViewportWidth] * (1.0f / m_FrameIndex));
			//m_ImageData[x + y * m_ViewportWidth] = accumulatedPixelColor.RGBA();
		}
	}

#endif


	m_FrameIndex++;

	image.SetImageData(m_ImageData);

	auto finished = std::chrono::high_resolution_clock::now();
	timeToRender = std::chrono::duration_cast<std::chrono::microseconds>(finished - start).count() * 0.001f;

	return true;
}

Color Renderer::PerPixel(int x, int y)
{
	Color pixelColor = Color::Black();

	int spp = 1;
	for (int sample = 0; sample < spp; sample++)
	{
		Ray ray;
		ray.Origin = m_Camera->GetPosition();
		ray.Direction = m_Camera->GetRayDirections()[x + y * m_Camera->GetViewportWidth()];

		Color throughput = Color::White();
		int bounces = 4;
		for (int bounce = 0; bounce < bounces; bounce++)
		{
			HitContext context = CastRay(ray);

			// Ray missed - add background color and break.
			if (!context.CorrespondsToValidHit)
			{
				pixelColor += throughput * Color::White();
				break;
			}

			Material material = m_Scene->Materials()[context.MaterialIndex];
			Color res = material.Sample(context);
			float pdf = material.Pdf(context);

			throughput *= res;

			ray.Origin = context.WorldPosition + 0.0001f * context.WorldNormal;
			ray.Direction = context.ToWorld(context.LocalIncidentDirection).normalized();
		}
	}

	return pixelColor * (1.0f / (float)spp);
}

HitContext Renderer::CastRay(Ray ray)
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
		HitContext context;
		context.CorrespondsToValidHit = false;
		return context;
	}
	
	Eigen::Vector3f normal = (ray.At(smallestT) - m_Scene->Spheres()[closestSphereIndex].Center).normalized();
	HitContext context(normal);
	Sphere sphere = m_Scene->Spheres()[closestSphereIndex];
	Color albedo = m_Scene->Materials()[sphere.MaterialIndex].Albedo;
	context.Albedo = albedo;
	context.HitDistance = smallestT;
	context.WorldPosition = ray.At(smallestT);
	context.LocalOutgoingDirection = context.ToLocal(-ray.Direction);

	return context;
}