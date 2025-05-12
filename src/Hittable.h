#pragma once

#include <glm/vec3.hpp>
#include "Material.h"

struct Hittable {

public:
	Hittable(glm::vec3 pos, float rad, Material mat) {
		m_Position = pos;
		m_Radius = rad;
		m_Mat = mat;
	}

	glm::vec3 m_Position;
	float m_Radius;
	Material m_Mat;
};