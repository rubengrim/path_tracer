#include "Renderer.h"
#include "Logger.h"
#include "Objects.h"
#include "Intersections.h"
#include "Utils.h"
#include "Random.h"

#include <cmath>
#include <chrono>
#include <thread>
#include <execution>
#include <algorithm>
#include <vector>
#include <iostream>

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

bool Renderer::Render(Image& image, Scene& scene, const Camera& camera, float& timeToRender, const RenderSettings& renderSettings)
{
	if (!renderSettings.Accumulate)
		m_FrameIndex = 1;

	if (!renderSettings.AccumulateForever && renderSettings.MaxAccumulatedSamples <= m_FrameIndex)
	{
		timeToRender = 0.0f;
		return false;
	}

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
			Color pixelColor = PerPixel(x, y, renderSettings);
			m_AccumulatedData[x + y * m_ViewportWidth] += pixelColor.ToVec3();

			Color accumulatedPixelColor(m_AccumulatedData[x + y * m_ViewportWidth] * (1.0f / m_FrameIndex));
			accumulatedPixelColor.Clamp();
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

Color Renderer::PerPixel(int x, int y, const RenderSettings& renderSettings)
{
	Ray ray;
	ray.Origin = m_Camera->GetPosition();
	ray.Direction = m_Camera->GetRayDirections()[x + y * m_Camera->GetViewportWidth()];

	Color pixelColor = Color::Black();
	Color throughput = Color::White();


	int maxBounces = 1;
	for (int bounce = 0; bounce < maxBounces; bounce++)
	{
		CastRay(ray);

		// Ray missed - add background color and break.
		if (!ray.Payload.RayHitSomething)
		{
			pixelColor += throughput * renderSettings.BackgroundColor;
			break;
		}

		// Create matrices that transform directions to and from the local coordinate system aligned to the normal of the hit.
		Eigen::Matrix3f localToWorld = Utils::CreateOrthogonalBasis(ray.Payload.WorldNormal);
		Eigen::Matrix3f worldToLocal = localToWorld.inverse();

		Sphere& sphere = m_Scene->Spheres()[ray.Payload.ObjectID];
		Material& material = m_Scene->Materials()[sphere.MaterialID];

		//if (material.EmitsLight)
		//	pixelColor += throughput * material.LightColor * material.LightIntensity;

		pixelColor += throughput * EvaluateDirectLight(ray, material, worldToLocal, localToWorld);

		// Sample an incident direction (in local space).
		Eigen::Vector3f incidentDirection;
		Color multiplier = BSDF::Lambertian::Sample(&incidentDirection, material.Albedo);
		throughput *= BSDF::Lambertian::Eval(material.Albedo, incidentDirection) * (1.0f / BSDF::Lambertian::Pdf(incidentDirection));
		//throughput *= multiplier;

		// Russian roulette
		float p = std::max(throughput.R, std::max(throughput.G, throughput.B));
		if (Random::Float() > p)
			break;

		throughput *= 1.0f / p;

		ray.Origin = ray.Payload.WorldPosition + 0.0001f * ray.Payload.WorldNormal;
		ray.Direction = localToWorld * incidentDirection;
	}

	return pixelColor;
}

void Renderer::CastRay(Ray& ray)
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

	if (smallestT == -1.0f) // No intersection found.
	{
		ray.Payload.RayHitSomething = false;
	}
	else
	{
		ray.Payload.RayHitSomething = true;
		ray.Payload.ObjectID = closestSphereIndex;
		ray.Payload.HitDistance = smallestT;
		ray.Payload.WorldPosition = ray.At(smallestT);
		Eigen::Vector3f normal = (ray.Payload.WorldPosition - m_Scene->Spheres()[closestSphereIndex].Center).normalized();
		ray.Payload.WorldNormal = normal;
	}
}

Color Renderer::EvaluateDirectLight(Ray ray, Material& material, Eigen::Matrix3f worldToLocal, Eigen::Matrix3f localToWorld)
{
	std::vector<int> emitters;
	for (int id = 0; id < m_Scene->Spheres().size(); id++)
	{
		if (m_Scene->GetMaterial(m_Scene->Spheres()[id].MaterialID).EmitsLight)
			emitters.push_back(id);
	}
	
	int randomEmitterID = emitters[Random::UInt(0, emitters.size() - 1)];
	Sphere emitter = m_Scene->Spheres()[randomEmitterID];

	Eigen::Vector3f pointOnSurface = emitter.SamplePointOnSurface();
	Eigen::Vector3f worldIncidentDirection = (pointOnSurface - ray.Payload.WorldPosition).normalized();
	worldIncidentDirection = (emitter.Center - ray.Payload.WorldPosition).normalized();

	Ray shadowRay;
	shadowRay.Origin = ray.Payload.WorldPosition + 0.001f * ray.Payload.WorldNormal;
	shadowRay.Direction = worldIncidentDirection;
	CastRay(shadowRay);

	if (shadowRay.Payload.ObjectID == randomEmitterID)
	{
		float P = (-worldIncidentDirection).dot(shadowRay.Payload.WorldNormal) / (pointOnSurface - ray.Payload.WorldPosition).dot(pointOnSurface - ray.Payload.WorldPosition);
		return BSDF::Lambertian::Eval(material.Albedo, worldToLocal * worldIncidentDirection) * material.LightColor * material.LightIntensity;
		//return BSDF::Lambertian::Eval(material.Albedo, worldToLocal * worldIncidentDirection) * material.LightColor * material.LightIntensity * P * (float)emitters.size();
	}
	else
		return Color::Black();
}