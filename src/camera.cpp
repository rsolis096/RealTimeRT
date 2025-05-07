#include "camera.h"

void Camera::initialize() {

    //image_height = int(image_width / aspect_ratio);
    //image_height = (image_height < 1) ? 1 : image_height;

    center = glm::vec3(0.f);

    // Determine viewport dimensions.
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (float(SCR_WIDTH) / SCR_HEIGHT);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    auto viewport_u = glm::vec3(viewport_width, 0, 0);
    auto viewport_v = glm::vec3(0, -viewport_height, 0);

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    pixel_delta_u = viewport_u / (float)SCR_WIDTH;
    pixel_delta_v = viewport_v / (float)SCR_HEIGHT;

    // Calculate the location of the upper left pixel.
    auto viewport_upper_left =
        center - glm::vec3(0, 0, focal_length) - viewport_u / 2.f - viewport_v / 2.f;
    pixel00_loc = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);
}


    /*
    color ray_color(const ray& r, const hittable& world) const {
        ...
    }
    */
