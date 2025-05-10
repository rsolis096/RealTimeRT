#pragma once

#include <glm/vec3.hpp>
#include "Material.h"

struct Hittable {

public:
	Hittable(glm::vec3 pos, glm::vec3 alb, float rad, float rf, Material mat, float fuzz = 1.0f) {
		m_Position = pos;
		m_Albedo = alb;
		m_Type = mat;
		m_Radius = rad;
		m_Refraction_Index = rf;
		m_Fuzz = fuzz;
	}

	glm::vec3 m_Position;
	glm::vec3 m_Albedo;
	float m_Radius;
	float m_Refraction_Index;
	Material m_Type;
	float m_Fuzz;
};