#pragma once
#include <vector>
#include "Primitives.h"

class Scene
{
public:
	inline void AddSpheres(std::vector<Sphere> spheres) { m_Spheres.insert(m_Spheres.end(), spheres.begin(), spheres.end()); };
	inline void AddSphere(Sphere sphere) { m_Spheres.push_back(sphere); }
	inline void DeleteSphere(int i) { m_Spheres.erase(m_Spheres.begin() + i); }

	inline std::vector<Sphere>& Spheres() { return m_Spheres; }

private:
	std::vector<Sphere> m_Spheres;
};