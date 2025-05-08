#version 430 core

in  vec2 fragUV;
out vec4 FragColor;

/* Struct Definitions */

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct hit_record {
    vec3 point;
    vec3 normal;
    float t;
    bool front_face;
};

struct Hittable {
    int type;
    vec3 sphereCenter;
    float sphereRadius;
};

struct Interval {
    float min;
    float max;
};

/* Constants */
const int MAX_HITTABLES = 2;
const float POS_MAX = 3.402823466e+38;
const float NEG_MAX = -3.402823466e+38;

/* Uniforms */
uniform int   hittableCount;
uniform Hittable hittables[MAX_HITTABLES];
uniform float uSeed; // Time used for random vallue seeding
uniform int SCR_WIDTH;
uniform int SCR_HEIGHT;

/* Utilities & Implementations */

float size(Interval i)  {
    return i.max - i.min;
}

bool contains(Interval i, float x)  {
    return i.min <= x && x <= i.max;
}

bool surrounds(Interval i, float x)  {
    return i.min < x && x < i.max;
}

float clamp(Interval i, float x) {
    if (x < i.min) return i.min;
    if (x > i.max) return i.max;
    return x;
}

// Returns a random float from [0, 1]
float random_float(vec2 st)
{
    float temp = fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
    return temp;
}

float random_range(vec2 st, float mn, float mx) {
    float r01 = random_float(st);       // in [0,1)
    return mn + r01 * (mx - mn);        // scale to [mn, mx)
}

vec3 random() {
    vec2 seed = gl_FragCoord.xy + vec2(uSeed, -uSeed);

    return vec3(
        random_float(seed + vec2(1.0, 0.0)), 
        random_float(seed + vec2(0.0, 0.0)), 
        random_float(seed + vec2(0.0, 1.0))
    );
}

vec3 random(float min, float max) {
    vec2 seed = gl_FragCoord.xy + vec2(uSeed, -uSeed);

    return vec3(
        random_range(seed + vec2(1.0, 0.0), min,max), 
        random_range(seed + vec2(0.0, 0.0), min,max), 
        random_range(seed + vec2(0.0, 1.0), min,max)
    );
}

// Returns a random unit vector
vec3 random_unit_vector(){
    vec2 seed = gl_FragCoord.xy + vec2(uSeed, -uSeed);
    float r1 = random_float(seed + vec2(1.0, 0.0));
    float r2 = random_float(seed + vec2(0.0, 0.0));
    float r3 = random_float(seed + vec2(0.0, 1.0));

    return normalize(vec3(r1, r2, r3));
}

vec3 random_on_hemisphere(inout vec3 normal) {
    vec3 on_unit_sphere = random_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return on_unit_sphere;
    else
        return -on_unit_sphere;
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

Ray make_ray(vec3 origin, vec3 direction) {
    Ray r;
    r.origin    = origin;
    r.direction = direction;
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

        return true;
}

vec3 ray_color(Ray r) {
    vec3 energy = vec3(1.0);   // start with 100% energy
    vec3 result = vec3(0.0);   // accumulated color

    const int MAX_DEPTH = 10;
    for (int depth = 0; depth < MAX_DEPTH; ++depth) {

        // 1) Find the closest hit within the scene
        hit_record rec;
        bool hit_anything = false;
        float closest_t = POS_MAX;
        Interval ray_t = Interval(0.001, POS_MAX);

        for (int i = 0; i < hittableCount; ++i) {
            if (intersectSphere(r, ray_t, rec, hittables[i]) && rec.t < closest_t) {
                hit_anything = true;
                closest_t    = rec.t;
            }
        }

        // 2) No hit found, this is sky
        if (!hit_anything) {
            vec3 unit_dir = normalize(r.direction);
            float t = 0.5 * (unit_dir.y + 1.0);
            vec3 sky = (1.0-t)*vec3(1.0) + t*vec3(0.5, 0.7, 1.0);
            result += energy * sky;
            return result;
        }

        // 3) Hit an object so scatter on this point
        vec3 target = rec.normal + random_unit_vector();
        r.origin    = rec.point;
        r.direction = target;

        //  Energy fall off for bounced lights
        energy *= 0.5;
    }

    return result;
}

void main() {
    ivec2 resolution = ivec2(SCR_WIDTH, SCR_HEIGHT);
    vec3  cam_origin = vec3(0.0);

    int   samples = 100;
    vec3  pixel_color = vec3(0.0);

    for (int i = 0; i < samples; ++i) {

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

        // Transform y to make image upright as per RTOW
        uv.y = 1.0 - uv.y; 

        float aspect = float(resolution.x) / float(resolution.y);
        float ndcX = 2.0 * uv.x - 1.0;    // map uv.x from [0,1] -> [-1,1]
        float ndcY = 1.0 - 2.0 * uv.y;    // map uv.y from [0,1] -> [1,-1] 

        vec3  dir = normalize(vec3(
            ndcX * aspect,
            ndcY,
            -1.0
        ));

        // 5) Trace Ray
        Ray r = make_ray(cam_origin, dir);
        pixel_color += ray_color(r);
    }

    // 6) average
    pixel_color /= float(samples);

    vec3 gammaCorrected = pow(pixel_color, vec3(1.0/2.2));

    FragColor = vec4(
        gammaCorrected,
        1.0
    );
}

