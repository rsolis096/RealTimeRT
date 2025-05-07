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
uniform vec4 uRandom; // 4 randomonly generated floats

/* Function Implementations*/

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

/*OTHER*/


void set_face_normal(in Ray r, inout hit_record rec, in vec3 outward_normal) {
    // Sets the hit record normal vector.
    // NOTE: the parameter `outward_normal` is assumed to have unit length.

    // Sphere
    rec.front_face = dot(r.direction, outward_normal) < 0;
    rec.normal = rec.front_face ? outward_normal : -outward_normal;
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

    bool hit_anything = false;
    hit_record rec;
    float closest_so_far = POS_MAX;
    Interval interval = {0, POS_MAX};

    for(int i = 0; i < 2; i++){
        bool res = intersectSphere(r, interval, rec, hittables[i]);
        if(res){
            hit_anything = true;
            closest_so_far = rec.t;
        }
    }

    if (hit_anything) {
        return 0.5 * (rec.normal + vec3(1,1,1));
    }
    

    vec3 unit_direction = normalize(r.direction);
    float a = 0.5*(unit_direction.y + 1.0);
    return (1.0-a)*vec3(1.0, 1.0, 1.0) + a*vec3(0.5, 0.7, 1.0);
}



void main() {

    // Hard code resolution
    ivec2 resolution = ivec2(1280, 720);
    vec3 cam_origin = vec3(0.0);

    // Get the current fragment (pixel)
    ivec2 pixel = ivec2(gl_FragCoord.xy);
    // Convert so (0,0) is upperleft instead of bottomleft:
    pixel.y = resolution.y - 1 - pixel.y;

    // normalize pixel coords to [0,1]
    vec2 uv = vec2(pixel) / vec2(resolution);

    // map to [-1,1] and preserve aspect
    float aspect = float(resolution.x) / float(resolution.y);

    vec3 dir = normalize(vec3(
        (2.0*uv.x - 1.0) * aspect,
        1.0 - 2.0*uv.y,
        -1.0  // look into the scene
    ));

    Ray r = make_ray(cam_origin, dir);
    vec3 col = ray_color(r);

    FragColor = vec4(col, 1.0);
}
