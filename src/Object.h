#pragma once

#include <vector>
#include "Material.h"

class Object {

public:

	glm::vec3 m_Position;
	float m_Radius;
	glm::vec3 m_Color;
	float m_MatId;

	std::vector<float> m_Vertices;
	std::vector<float> m_Indices;

	// Must be overridden by derived classes
	virtual int type_id() const = 0;

	Material m_Material;
	GPUMaterial GetGPUMaterial();

};
