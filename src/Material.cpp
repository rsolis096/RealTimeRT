#include "Material.h"

Material::Material()
{
	m_Type = LAMBERTIAN;
	m_RefractionIndex = 1.0f;
	m_Fuzz = 0.0f;
	m_Albedo = glm::vec3(1.f);
}

// Lambertian
Material::Material(const glm::vec3& albedo) {
	m_Type = LAMBERTIAN;
	m_RefractionIndex = 1.0f;
	m_Fuzz = 0.0f;
	m_Albedo = albedo;
}

// Metal
Material::Material(const glm::vec3& albedo, const float fuzz) {
	m_Type = METAL;
	m_RefractionIndex = 1.0f;
	m_Fuzz = fuzz;
	m_Albedo = albedo;
}

// Dielectric
Material::Material(const float rf) {
	m_Type = DIELECTRIC;
	m_RefractionIndex = rf;
	m_Fuzz = 0.0f;
	m_Albedo = glm::vec3(1.f);
}