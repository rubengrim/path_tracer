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

// Parallell for_each() loops are MSVS-only, so allow multithreading on Windows only.
#define MULTI_THREADING
#if defined _PLATFORM_WINDOWS && defined MULTI_THREADING

	std::vector<int> yValues;
	for (int y = 0; y < m_ViewportHeight; y++)
		yValues.push_back(y);

	std::for_each(std::execution::par, yValues.begin(), yValues.end(), [&](int& y)
	{
		for (int x = 0; x < m_ViewportWidth; x++)
		{
			Eigen::Vector3f pixelColor = PerPixel(x, y, renderSettings);
			m_AccumulatedData[x + y * m_ViewportWidth] += pixelColor;

			Eigen::Vector3f accumulatedPixelColor(m_AccumulatedData[x + y * m_ViewportWidth] * (1.0f / m_FrameIndex));
			Eigen::Vector3f clamped = Utils::ClampV(accumulatedPixelColor, 0.0f, 1.0f);
			m_ImageData[x + y * m_ViewportWidth] = Utils::Color::FloatToRGBA(clamped);
		}
	});

#else

	for (int y = 0; y < m_ViewportHeight; y++)
	{
		for (int x = 0; x < m_ViewportWidth; x++)
		{
      Eigen::Vector3f pixelColor = PerPixel(x, y, renderSettings);
      m_AccumulatedData[x + y * m_ViewportWidth] += pixelColor;
      Eigen::Vector3f accumulatedPixelColor(m_AccumulatedData[x + y * m_ViewportWidth] * (1.0f / m_FrameIndex));
      Eigen::Vector3f clamped = Utils::ClampV(accumulatedPixelColor, 0.0f, 1.0f);
      m_ImageData[x + y * m_ViewportWidth] = Utils::Color::FloatToRGBA(clamped);
		}
	}

#endif

	m_FrameIndex++;

	image.SetImageData(m_ImageData);

	auto finished = std::chrono::high_resolution_clock::now();
	timeToRender = std::chrono::duration_cast<std::chrono::microseconds>(finished - start).count() * 0.001f;

	return true;
}

Eigen::Vector3f Renderer::PerPixel(int x, int y, const RenderSettings& renderSettings)
{
	Ray ray;
	ray.Origin = m_Camera->GetPosition();
	ray.Direction = m_Camera->GetRayDirections()[x + y * m_Camera->GetViewportWidth()];

	Eigen::Vector3f pixelColor = Utils::Color::Black();
	Eigen::Vector3f throughput = Utils::Color::White();

	int maxBounces = 40;
	for (int bounce = 0; bounce < maxBounces; bounce++)
	{
		CastRay(ray);

		// Ray missed - add background color and break.
		if (!ray.Payload.RayHitSomething)
		{
			pixelColor += throughput.cwiseProduct(renderSettings.BackgroundColor);
			break;
		}

		// Create matrices that transform directions to and from the local coordinate system aligned to the normal of the hit.
		Eigen::Matrix3f localToWorld = Utils::CreateOrthogonalBasis(ray.Payload.WorldNormal);
		Eigen::Matrix3f worldToLocal = localToWorld.inverse();

		Sphere& sphere = m_Scene->Spheres()[ray.Payload.ObjectID];
		Material& material = m_Scene->Materials()[sphere.MaterialID];

    if (bounce == 0 && material.EmitsLight)
    {
      pixelColor += throughput.cwiseProduct(material.LightColor * material.LightIntensity);
    }

		Eigen::Vector3f direct = EvaluateDirectLight(ray.Payload, material, worldToLocal, localToWorld);
		pixelColor += throughput.cwiseProduct(direct);

		// Sample an incident direction (in local space).
		Eigen::Vector3f incidentDirection;
		Eigen::Vector3f weight = BSDF::Lambertian::Sample(material.Albedo, &incidentDirection);
    throughput = throughput.cwiseProduct(weight);
		//throughput = throughput.cwiseProduct(BSDF::Lambertian::Eval(material.Albedo, incidentDirection)) * (1.0f / BSDF::Lambertian::Pdf(incidentDirection));

		// Russian roulette
		float p = std::max(throughput.x(), std::max(throughput.y(), throughput.z()));
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

Eigen::Vector3f Renderer::EvaluateDirectLight(RayPayload refPayload, Material& material, Eigen::Matrix3f worldToLocal, Eigen::Matrix3f localToWorld)
{
	std::vector<int> emitters;
	for (int id = 0; id < m_Scene->Spheres().size(); id++)
	{
		if (m_Scene->GetMaterial(m_Scene->GetSphere(id).MaterialID).EmitsLight)
    {
			emitters.push_back(id);
    }
	}

  if (emitters.size() == 0)
  {
    return Utils::Color::Black();
  }

	int chosenEmitterID = emitters[Random::UInt(0, emitters.size() - 1)];
	Sphere chosenEmitter = m_Scene->GetSphere(chosenEmitterID);
	Material chosenEmitterMaterial = m_Scene->GetMaterial(chosenEmitter.MaterialID);

  if (refPayload.ObjectID == chosenEmitterID)
  {
    return Utils::Color::Black();
  }

	Eigen::Vector3f pointOnSurface; 
  float lightPdf = chosenEmitter.SamplePointOnSurface(refPayload.WorldPosition, &pointOnSurface);
  if (lightPdf == 0.0f)
  {
    return Utils::Color::Black();
  }

	Eigen::Vector3f worldIncidentDirection = (pointOnSurface - refPayload.WorldPosition).normalized();
  
  if (worldIncidentDirection.dot(refPayload.WorldNormal) <= 0.0f)
  {
    return Utils::Color::Black();
  }

	Ray shadowRay;
	shadowRay.Origin = refPayload.WorldPosition + 0.0001f * refPayload.WorldNormal;
	shadowRay.Direction = worldIncidentDirection;
	CastRay(shadowRay);

	if (shadowRay.Payload.ObjectID == chosenEmitterID)
	{
//		float P = (-worldIncidentDirection).dot(shadowRay.Payload.WorldNormal) / (chosenEmitter.Center - refPayload.WorldPosition).dot(chosenEmitter.Center - refPayload.WorldPosition);
//		Eigen::Vector3f direct = BSDF::Lambertian::Eval(material.Albedo, worldToLocal * worldIncidentDirection).cwiseProduct(chosenEmitterMaterial.LightColor) * chosenEmitterMaterial.LightIntensity * P * (float)emitters.size();
    Eigen::Vector3f received = chosenEmitterMaterial.LightColor * chosenEmitterMaterial.LightIntensity;
    Eigen::Vector3f direct = BSDF::Lambertian::Eval(material.Albedo, worldToLocal * worldIncidentDirection).cwiseProduct(received) / lightPdf * (float)emitters.size(); 

    direct = Utils::ClampVMin(direct, 0.0f);
		return direct;
	}
	else
  {
		return Utils::Color::Black();
  }
}
