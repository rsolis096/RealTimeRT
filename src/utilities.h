#pragma once

#include <glm/glm.hpp>
#include <random>

// Returns a random float in range [0, 1)
inline float random_float() {
    static std::uniform_real_distribution<float> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

// Returns a random float in [min,max).
inline float random_float(float min, float max) {
    return min + (max - min) * random_float();
}

// Returns a random vec3 with values in range [0.1]
inline glm::vec3 random_vec() {
    return glm::vec3(random_float(), random_float(), random_float());
}

// Returns a random vec3 with values in range [min,max).
inline glm::vec3 random_vec(float min, float max) {
    return glm::vec3(random_float(min, max), random_float(min, max), random_float(min, max));
}

// Fills an array of floats with random values
inline void set_urandom(std::vector<float>& in) {
    for (int i = 0; i < in.size(); i++) {
        in[i] = random_float();
    }
}