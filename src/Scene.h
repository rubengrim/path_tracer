#pragma once
#include <vector>
#include "Primitives.h"

class Scene
{
public:
	inline void AddSpheres(std::vector<Sphere> spheres) { m_Spheres.insert(m_Spheres.end(), spheres.begin(), spheres.end()); };

	inline const std::vector<Sphere>& GetSpheres() { return m_Spheres; }

private:
	std::vector<Sphere> m_Spheres;
};