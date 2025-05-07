#pragma once

#include <random>

const float pi = 3.1415926535;
const float infinity = std::numeric_limits<float>::infinity();


inline float degrees_to_radians(float degrees) {
    return degrees * pi / 180.0;
}

inline float random_float() {
    static std::uniform_real_distribution<float> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline float random_float(float min, float max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_float();
}

// Fills an array of floats with random values
inline void set_urandom(std::vector<float>& in) {
    for (int i = 0; i < in.size(); i++) {
        in[i] = random_float();
    }
}