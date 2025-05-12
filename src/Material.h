#pragma once

#include <glm/glm.hpp>

enum Material_Type {
	LAMBERTIAN,	// 0
	METAL,		// 1
	DIELECTRIC,	// 2
};

struct Material {
	Material_Type m_Type;
	float m_Fuzz;
	float m_RefractionIndex;
	glm::vec3 m_Albedo;

	// Default White Sphere
	Material();

	// Lambertian
	Material(const glm::vec3& albedo);

	// Metal
	Material(const glm::vec3& albedo, const float fuzz);

	// Dielectric
	Material(const float rf);


};
