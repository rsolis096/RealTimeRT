#pragma once

#include "Object.h"
#define PI 3.141592653589793238462643383279502884197

struct GPUSphere {           // 32 bytes (two vec4s)
	glm::vec4 center_radius; // xyz = center, w = radius
	glm::vec4 color_matId;   // rgb = color,  w = matId (as float)
};

class Sphere : public Object {

	public:
		// Type 0 is considered a sphere
		int type_id() const override { return 0; }

		Sphere(glm::vec3 pos, glm::vec3 color, float rad);

		void BuildSphere();

		GPUSphere GetGPUSphere();
	
};

