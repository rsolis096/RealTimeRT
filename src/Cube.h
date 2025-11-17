#pragma once

#include "Object.h"

struct Cube : public Object {
    glm::vec3 minCorner;
    glm::vec3 maxCorner;
    glm::vec3 color;
    int m_MatId;

    // overrides

    /*
    AABB bounding_box() const override { }
    */

    // Type 1 is considered a Cube
    int type_id() const override { return 1; }
};

struct GPUCube {             // 32 bytes (two vec4s)
    glm::vec4 min_pad;       // xyz = minCorner, w = unused
    glm::vec4 max_matId;     // xyz = maxCorner, w = matId (as float)
};

