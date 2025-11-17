#include "Material.h"

// Default White Sphere
Material::Material()
{
	m_Type = LAMBERTIAN;
	m_RefractionIndex = 0.0f;
	m_Fuzz = 0.0f;
	m_Albedo = glm::vec3(1.f);
}

//  Lambertian
Material::Material(const glm::vec3& albedo) {
	m_Type = LAMBERTIAN;
	m_RefractionIndex = 0.0f;
	m_Fuzz = 0.0f;
	m_Albedo = albedo;
}

// Metal
Material::Material(const glm::vec3& albedo, const float fuzz) {
	m_Type = METAL;
	m_RefractionIndex = 0.0f;
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

// Lambertian
Material Material::MakeLambertian(const glm::vec3& albedo) {
	
	Material mat;
	mat.m_Type = LAMBERTIAN;
	mat.m_RefractionIndex = 0.0f;
	mat.m_Fuzz = 0.0f;
	mat.m_Albedo = albedo;
	return mat;
}

// Metal
Material Material::MakeMetal(const glm::vec3& albedo, const float fuzz) {
	
	Material mat;
	mat.m_Type = METAL;
	mat.m_RefractionIndex = 0.0f;
	mat.m_Fuzz = fuzz;
	mat.m_Albedo = albedo;
	return mat;
}

// Dielectric
Material Material::MakeDielectric(const float rf) {

	Material mat;
	mat.m_Type = DIELECTRIC;
	mat.m_RefractionIndex = rf;
	mat.m_Fuzz = 0.0f;
	mat.m_Albedo = glm::vec3(1.f);
	return mat;
}
