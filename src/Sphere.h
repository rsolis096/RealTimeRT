#pragma once

#include "Hittable.h"

struct Sphere : public Hittable {
	glm::vec3 m_Position;
	float m_Radius;
	glm::vec3 m_Color;
	int m_Mat; // Index into materials array


	/*
	AABB bounding_box() const override { }
	*/

	// Type 0 is considered a sphere
	int type_id() const override { return 0; }

	Sphere(glm::vec3 pos, float rad, int mat) {
		m_Position = pos;
		m_Radius = rad;
		m_Mat = mat;
		m_Color = glm::vec3(0.f);
	}


};

struct GPUSphere {           // 32 bytes (two vec4s)
	glm::vec4 center_radius; // xyz = center, w = radius
	glm::vec4 color_matId;   // rgb = color,  w = matId (as float)
};