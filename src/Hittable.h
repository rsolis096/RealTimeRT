#pragma once

#include <glm/vec3.hpp>
#include "Material.h"

class AABB;

struct Hittable {

	//virtual AABB bounding_box() const = 0;
	virtual int type_id() const = 0;

	/*
	Hittable(glm::vec3 pos, float rad, Material mat) {
		m_Position = pos;
		m_Radius = rad;
		m_Mat = mat;
	}

	glm::vec3 m_Position;
	float m_Radius;
	Material m_Mat;
	*/
};

struct GPUHittable {        // 8 bytes
	int type;                // 0 = sphere, 1 = cube
	int index;               // index into spheres[] or cubes[]
};