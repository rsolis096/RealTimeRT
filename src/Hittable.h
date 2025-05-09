#pragma once

#include <glm/vec3.hpp>
#include "Material.h"

struct Hittable {

public:
	Hittable(glm::vec3 pos, glm::vec3 alb, float rad, Material mat) {
		m_Position = pos;
		m_Albedo = alb;
		m_Type = mat;
		m_Radius = rad;
	}

	glm::vec3 m_Position;
	glm::vec3 m_Albedo;
	float m_Radius;
	Material m_Type;
};