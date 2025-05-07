#pragma once
#include <iostream>
#include <string>
#include <glm/glm.hpp>

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

class Camera {
public:
    float aspect_ratio = 1.0;  // Ratio of image width over height
    void initialize();

private:
    glm::vec3 center;         // Camera center
    glm::vec3 pixel00_loc;    // Location of pixel 0, 0
    glm::vec3   pixel_delta_u;  // Offset to pixel to the right
    glm::vec3   pixel_delta_v;  // Offset to pixel below


    //color ray_color(const ray& r, const hittable& world);
};