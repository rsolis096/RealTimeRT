#pragma once

#include <glm/glm.hpp>

enum Material_Type {
	LAMBERTIAN,	// 0
	METAL,		// 1
	DIELECTRIC,	// 2
};

// std430 32 bytes
struct GPUMaterial {
	glm::vec4 albedo_fuzz;   // rgb = albedo, w = fuzz
	glm::vec4 type_ref_pad;  // x = type (as float), y = ior, z,w = padding
};

class Material{

public:
	Material_Type m_Type;
	float m_Fuzz;
	float m_RefractionIndex;
	glm::vec3 m_Albedo;

	// Default White Sphere
	Material();

	// Lambertian
	Material(const glm::vec3& albedo);
	static Material MakeLambertian(const glm::vec3& albedo);

	// Metal
	Material(const glm::vec3& albedo, const float fuzz);
	static Material MakeMetal(const glm::vec3& albedo, const float fuzz);

	// Dielectric
	Material(const float rf);
	static Material MakeDielectric(const float rf);

};


