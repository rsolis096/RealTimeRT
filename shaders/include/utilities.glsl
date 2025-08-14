// Convert degrees to radians
float degrees_to_radians(float degrees) {
    return degrees * pi / 180.0;
}

// Return a pseudo-random float in range [0,1]
float random_float(vec2 st)
{
    float temp = fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
    return temp;
}

// Return random float in [mn, mx]
float random_range(vec2 st, float mn, float mx) {
    float r01 = random_float(st);       // in [0,1)
    return mn + r01 * (mx - mn);        // scale to [mn, mx)
}

// Return random vec3 with values in range [0,1]
vec3 random() {
    vec2 seed = pixel_coords.xy + vec2(iSeed, -iSeed);

    return vec3(
        random_float(seed + vec2(1.0, 0.0)), 
        random_float(seed + vec2(0.0, 0.0)), 
        random_float(seed + vec2(0.0, 1.0))
    );
}

// Return random vec3 with values in range [0,1]
vec3 random(float min, float max) {
    vec2 seed = pixel_coords.xy + vec2(iSeed, -iSeed);

    return vec3(
        random_range(seed + vec2(1.0, 0.0), min,max), 
        random_range(seed + vec2(0.0, 0.0), min,max), 
        random_range(seed + vec2(0.0, 1.0), min,max)
    );
}

// Return random unit-length vector
vec3 random_unit_vector(){
    vec2 seed = pixel_coords.xy + vec2(iSeed, -iSeed);
    float r1 = random_float(seed + vec2(1.0, 0.0));
    float r2 = random_float(seed + vec2(0.0, 0.0));
    float r3 = random_float(seed + vec2(0.0, 1.0));

    return normalize(vec3(r1, r2, r3));
}

// Return random vector on hemisphere around normal
vec3 random_on_hemisphere(inout vec3 normal) {

    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

// Return random point inside unit disk (for defocus)
vec3 random_in_unit_disk() {
    vec3 p;
    do {
        p = vec3(
            random_range(pixel_coords.xy + vec2(iSeed, -iSeed), -1.0, 1.0),
            random_range(pixel_coords.yx + vec2(-iSeed, iSeed), -1.0, 1.0),
            0.0
        );
    } while (dot(p,p) >= 1.0);
    return p;
}


vec2 sample_square2D() {
    vec2 r = vec2(
      random_float(pixel_coords.xy + vec2(iSeed, -iSeed)),  // or however you seed
      random_float(pixel_coords.xy + vec2(-iSeed, iSeed))
    );
    return r - 0.5;
}

vec3 defocus_disk_sample(vec3 origin) {
    // Returns a random point in the camera defocus disk.
    vec3 p = random_in_unit_disk();
    return origin + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
}

float linear_to_gamma(float linear_component)
{
    if (linear_component > 0)
        return sqrt(linear_component);

    return 0;
}

vec3 reflect(vec3 v, vec3 n) {
    return v - 2*dot(v,n)*n;
}

bool near_zero(vec3 v) {
    const float s = 1e-8;
    // abs(v) < vec3(s) yields a bvec3, and all(...) checks all components
    return all( lessThan( abs(v), vec3(s) ) );
}

float reflectance(float cosine, float refraction_index) {
        // Use Schlick's approximation for reflectance.
        float r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 + (1-r0)* pow((1 - cosine),5);
}