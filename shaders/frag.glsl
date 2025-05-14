#version 430 core

in  vec2 fragUV;
out vec4 FragColor;

/* Struct Definitions */

struct Ray {
    vec3 origin;      // Ray start point
    vec3 direction;   // Ray direction vector
};

struct Material {
    int type;              // 0=Lambertian,1=Metal,2=Dielectric
    vec3 albedo;           // Base color or reflectance
    float refraction_index;// Index of refraction for dielectrics
    float fuzz;            // Fuzziness for metal reflections
};

struct hit_record {
    vec3 point;       // Intersection point
    vec3 normal;      // Surface normal at hit
    float t;          // Ray parameter at hit
    bool front_face;  // Did we hit front-facing side?
    Material mat;     // Material properties at hit
};

struct Hittable {
    vec3 sphereCenter; // Sphere center position
    float sphereRadius;// Sphere radius
    Material mat;      // Material for this sphere
};

struct Interval {
    float min; // Lower bound (t_min)
    float max; // Upper bound (t_max)
};

struct Camera {
    vec3 lookfrom;      // Eye position
    vec3 lookat;        // Look-at point or direction
    vec3 vup;           // World up vector
    float vfov;         // Vertical field of view (deg)
    float defocus_angle;// Aperture angle for blur
    float focus_dist;   // Focus distance for depth-of-field
};

/* Constants*/
const int MAX_HITTABLES = 100;
const float POS_MAX = 3.402823466e+38;   // Max positive float
const float NEG_MAX = -3.402823466e+38;  // Max negative float
const float pi = 3.14159265358979323846;
vec3   defocus_disk_u;       // Disk X basis for defocus
vec3   defocus_disk_v;       // Disk Y basis for defocus


/* Uniforms */
uniform Camera cam;
uniform int   hittableCount;
uniform Hittable hittables[MAX_HITTABLES];
uniform float uSeed; // Time used for random vallue seeding
uniform int SCR_WIDTH;
uniform int SCR_HEIGHT;
uniform int SAMPLES;;
uniform int MAX_DEPTH;

/* Globals */
ivec2 resolution = ivec2(SCR_WIDTH, SCR_HEIGHT);


/* Utilities & Implementations */

// Convert degrees to radians
float degrees_to_radians(float degrees) {
    return degrees * pi / 180.0;
}

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
    vec2 seed = gl_FragCoord.xy + vec2(uSeed, -uSeed);

    return vec3(
        random_float(seed + vec2(1.0, 0.0)), 
        random_float(seed + vec2(0.0, 0.0)), 
        random_float(seed + vec2(0.0, 1.0))
    );
}

// Return random vec3 with values in range [0,1]
vec3 random(float min, float max) {
    vec2 seed = gl_FragCoord.xy + vec2(uSeed, -uSeed);

    return vec3(
        random_range(seed + vec2(1.0, 0.0), min,max), 
        random_range(seed + vec2(0.0, 0.0), min,max), 
        random_range(seed + vec2(0.0, 1.0), min,max)
    );
}

// Return random unit-length vector
vec3 random_unit_vector(){
    vec2 seed = gl_FragCoord.xy + vec2(uSeed, -uSeed);
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
            random_range(gl_FragCoord.xy + vec2(uSeed, -uSeed), -1.0, 1.0),
            random_range(gl_FragCoord.yx + vec2(-uSeed, uSeed), -1.0, 1.0),
            0.0
        );
    } while (dot(p,p) >= 1.0);
    return p;
}

void set_face_normal(in Ray r, inout hit_record rec, in vec3 outward_normal) {
    // Sets the hit record normal vector.
    // NOTE: the parameter `outward_normal` is assumed to have unit length.

    // Sphere
    rec.front_face = dot(r.direction, outward_normal) < 0;
    rec.normal = rec.front_face ? outward_normal : -outward_normal;
}

vec2 sample_square2D() {
    vec2 r = vec2(
      random_float(gl_FragCoord.xy + vec2(uSeed, -uSeed)),  // or however you seed
      random_float(gl_FragCoord.xy + vec2(-uSeed, uSeed))
    );
    return r - 0.5;
}

vec3 defocus_disk_sample(vec3 origin) {
    // Returns a random point in the camera defocus disk.
    vec3 p = random_in_unit_disk();
    return origin + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
}

Ray make_ray(vec3 origin, vec3 direction, vec3 filmPoint) {
    Ray r;
    r.origin    = (cam.defocus_angle <= 0) ? origin : defocus_disk_sample(origin);
    r.direction = normalize(filmPoint - r.origin);
    return r;
}

vec3 ray_at(Ray r, float t) {
    return r.origin + t * r.direction;
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

// Material Scattering
bool scatter(
    in  Ray        r_in,        // incoming ray (read-only)
    in  hit_record rec,         // hit info (read-only)
    out vec3       attenuation, // how the color is attenuated
    out Ray        scattered   // the scattered ray
){
    
    //mat. == 0 is lambertian
    //matType == 1 is dielectric

    // Lambertian
    if(rec.mat.type == 0) {
        vec3 scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (near_zero(scatter_direction))
            scatter_direction = rec.normal;
            
        scattered.origin = rec.point;
        scattered.direction = scatter_direction;        
        
        attenuation = rec.mat.albedo;
        return true;
    }

    // Metal
    if(rec.mat.type == 1) {
        vec3 reflected = reflect(r_in.direction, rec.normal);
        reflected = normalize(reflected) + (rec.mat.fuzz * random_unit_vector());

        scattered.origin = rec.point;
        scattered.direction = reflected;

        attenuation = rec.mat.albedo;
        return (dot(scattered.direction, rec.normal) > 0);
    }

    // Dielectric
    if(rec.mat.type == 2){
        attenuation = vec3(1.0);
        float ri = rec.front_face ? (1.0/rec.mat.refraction_index) : rec.mat.refraction_index;

        vec3 unit_direction = normalize(r_in.direction);
        float cos_theta = min(dot(-unit_direction, rec.normal), 1.0);
        float sin_theta = sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_float(gl_FragCoord.xy))
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered.origin = rec.point;
        scattered.direction = direction;
        return true;
    }

    return false;

}

bool intersectSphere(Ray r, inout Interval ray_t, out hit_record rec, Hittable obj) {
        vec3 oc = obj.sphereCenter - r.origin;
        float a = dot(r.direction, r.direction);
        float h = dot(r.direction, oc);
        float c = dot(oc, oc) - obj.sphereRadius*obj.sphereRadius;

        float discriminant = h*h - a*c;
        if (discriminant < 0)
            return false;

        float sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        float root = (h - sqrtd) / a;
        if (!surrounds(ray_t, root)) {
            root = (h + sqrtd) / a;
            if (!surrounds(ray_t, root))
                return false;
        }

        rec.t = root;
        rec.point = ray_at(r, rec.t);
        vec3 outward_normal = (rec.point - obj.sphereCenter) / obj.sphereRadius;
        set_face_normal(r, rec, outward_normal);
        rec.mat = obj.mat;

        return true;
}


vec3 ray_color(Ray r) {
    vec3 throughput = vec3(1.0);   // cumulative attenuation
    vec3 result  = vec3(0.0);   // what we’ll return

    for (int depth = 0; depth < MAX_DEPTH; ++depth) {
        
        // 1) cast ray r into the scene
        hit_record closest_rec;
        bool hit_something = false;
        float closest_hit = POS_MAX;
        Interval ray_t = Interval(0.001, POS_MAX);

        for (int i = 0; i < hittableCount; ++i) {
            hit_record temp_rec;

            // Find closest hit, save it to closest_rec
            Interval testT = Interval(0.001, closest_hit);
            if (intersectSphere(r, testT, temp_rec, hittables[i])) {
                hit_something = true;
                closest_hit     = temp_rec.t;   // tighten *our* closestT
                closest_rec      = temp_rec;     // save the record
            }
        }

        // 2) if we missed, add sky and break
        if (!hit_something) {
            vec3 unit_dir = normalize(r.direction);
            float t = 0.5 * (unit_dir.y + 1.0);
            vec3 sky = (1.0-t)*vec3(1.0) + t*vec3(0.5, 0.7, 1.0);
            result += throughput * sky;
            return result;
        }

        // 3) we hit something: scatter
        vec3 attenuation;
        Ray scattered;
        if (scatter(r, closest_rec, attenuation, scattered)) {
            // accumulate the attenuation
            throughput *= attenuation;
            // continue tracing the scattered ray
            r = scattered;
        }

    }

    return result;
}

vec3 update_camera(vec2 uv){

    // Initialize the camera

    vec3 camDir = normalize(cam.lookfrom - cam.lookat);
    vec3 camRight = normalize(cross(cam.vup, camDir));
    vec3 camUp = cross(camDir, camRight); // What is considered up relative to cam


    float aspect      = float(resolution.x) / float(resolution.y);
    float theta       = radians(cam.vfov);
    float half_h      = tan(theta * 0.5); // half-height of the image plane
    float half_w      = aspect * half_h; // Adjust width for to keep aspect ratio


    vec3 horizontal  = 2.0 * half_w * camRight * cam.focus_dist;
    vec3 vertical    = 2.0 * half_h * camUp  * cam.focus_dist;

    vec3 lower_left = cam.lookfrom
                     - camDir * cam.focus_dist
                     - camRight * half_w  * cam.focus_dist
                     - camUp    * half_h  * cam.focus_dist;                

    float defocus_radius = cam.focus_dist * tan(degrees_to_radians(cam.defocus_angle / 2));
    defocus_disk_u = camRight * defocus_radius;
    defocus_disk_v = camUp * defocus_radius;

    return lower_left 
        + uv.x * horizontal   
        + uv.y * vertical;

}

void main() {

    // Initialize ray tracing properties
    vec3  pixel_color = vec3(0.0);

    for (int i = 0; i < SAMPLES; ++i) {


        // 1) Create a seed for jitter vec generation
        float fi = float(i);
        vec2 seed = gl_FragCoord.xy + vec2(uSeed + fi, uSeed - fi);

        // 2) Create jitter offset for anti-aliasing [-0.5, 0.5]
        vec2 jitter = vec2(
            random_float(seed + vec2(1.0,0.0)),
            random_float(seed + vec2(0.0,1.0))
        ) - 0.5;

        // 3) Set pixel coords with offset for current sample
        vec2 pixel_coords = vec2(gl_FragCoord.xy) + jitter;

        // 4) Convert pixel coords to UV coords
        vec2 uv = pixel_coords / vec2(resolution);


        vec3 filmPoint = update_camera(uv);
        vec3 dir       = normalize(filmPoint - cam.lookfrom);
        Ray  r         = make_ray(cam.lookfrom, dir, filmPoint);

        pixel_color += ray_color(r);
    }


    // 6) average
    pixel_color /= float(SAMPLES);



    vec3 gammaCorrected = pow(pixel_color, vec3(1.0/2.2));

    FragColor = vec4(
        gammaCorrected,
        1.0
    );
}

