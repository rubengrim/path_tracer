#pragma once

#include "Objects.h"
#include "Material.h"

#include <vector>

class Scene
{
public:
	inline Sphere& GetSphere(int id) { return m_Spheres[id]; }
	inline void AddSpheres(std::vector<Sphere> spheres) { m_Spheres.insert(m_Spheres.end(), spheres.begin(), spheres.end()); };
	inline void AddSphere(Sphere sphere) { m_Spheres.push_back(sphere); }
	inline void DeleteSphere(int i) { m_Spheres.erase(m_Spheres.begin() + i); }

	inline Material& GetMaterial(int id) { return m_Materials[id]; }
	inline void AddMaterials(std::vector<Material> material) { m_Materials.insert(m_Materials.end(), material.begin(), material.end()); };
	inline void AddMaterial(Material material) { m_Materials.push_back(material); }
	inline void DeleteMaterial(int i) { m_Materials.erase(m_Materials.begin() + i); }

	inline std::vector<Material>& Materials() { return m_Materials; }
	inline std::vector<Sphere>& Spheres() { return m_Spheres; }

	inline Material& GetMaterialOfSphere(int sphereIndex) { return m_Materials[m_Spheres[sphereIndex].MaterialID]; }

private:
	std::vector<Sphere> m_Spheres;
	std::vector<Material> m_Materials;
};
