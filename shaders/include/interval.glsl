
#ifndef INTERVAL_GLSL
#define INTERVAL_GLSL

struct Interval {
    float min; // Lower bound (t_min)
    float max; // Upper bound (t_max)
};

// Size of an interval
float size(Interval i)  {
    return i.max - i.min;
}

// Check if x is within [min, max]
bool contains(Interval i, float x)  {
    return i.min <= x && x <= i.max;
}

// Check if x is within (min, max)
bool surrounds(Interval i, float x)  {
    return i.min < x && x < i.max;
}

// Clamp x to the interval [min, max]
float clamp(Interval i, float x) {
    if (x < i.min) return i.min;
    if (x > i.max) return i.max;
    return x;
}

#endif // Must end with a newline
