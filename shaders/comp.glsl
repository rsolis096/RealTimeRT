#version 430 core
#extension GL_ARB_shading_language_include : require

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

// Output image texture, bound to image unit 0
layout(rgba32f, binding = 0) uniform image2D imgOutput;


/* Constants */
const int MAX_SPHERES = 100;
const float POS_MAX = 3.402823466e+38;   // Max positive float
const float NEG_MAX = -3.402823466e+38;  // Max negative float
const float pi = 3.14159265358979323846;
vec3   defocus_disk_u;       // Disk X basis for defocus
vec3   defocus_disk_v;       // Disk Y basis for defocus
float iSeed;
ivec2 pixel_coords; // replaces gl_fragcoords;

/* Includes (constants defined in main for now) */
#include "/utilities.glsl"
#include "/interval.glsl"
#include "/camera.glsl"
#include "/aabb.glsl"

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

struct Sphere {
    vec3 sphereCenter;  // Sphere center position
    float sphereRadius; // Sphere radius
    Material mat;       // Material for this sphere
};

// Packed Spheres from CPU
layout(std430, binding = 0) buffer SpheresBuf {
    struct {
        vec4 center_radius;            // xyz center, w radius
        vec4 color_matId;              // rgb color,  w matId
    } inSpheres[];
};
Sphere spheres[MAX_SPHERES];

// Packed Materials from CPU
layout(std430, binding = 1) buffer MaterialsBuf {
    struct {
	    vec4 albedo_fuzz;   // rgb = albedo, w = fuzz
	    vec4 type_ref_pad;  // x = type (as float), y = ior, z,w = padding
    } inMaterials[];
};

/* Uniforms */
uniform Camera cam;
uniform int uSphereCount;
uniform int uMaterialsCount;

uniform float uSeed; // Time used for random value seeding
uniform int SCR_WIDTH;
uniform int SCR_HEIGHT;
uniform int SAMPLES;
uniform int MAX_DEPTH;

/* Utilities & Implementations */

// Put in sphere.glsl
void set_face_normal(in Ray r, inout hit_record rec, in vec3 outward_normal) {
    // Sets the hit record normal vector.
    // NOTE: the parameter `outward_normal` is assumed to have unit length.

    // Sphere
    rec.front_face = dot(r.direction, outward_normal) < 0;
    rec.normal = rec.front_face ? outward_normal : -outward_normal;
}

// Put in camera.glsl
Ray make_ray(in Camera camera, vec3 origin, vec3 direction, vec3 filmPoint) {
    Ray r;
    r.origin    = (camera.defocus_angle <= 0) ? origin : defocus_disk_sample(origin);
    r.direction = normalize(filmPoint - r.origin);
    return r;
}

vec3 ray_at(Ray r, float t) {
    return r.origin + t * r.direction;
}

// Material Scattering put in material.glsl
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

        if (cannot_refract || reflectance(cos_theta, ri) > random_float(pixel_coords.xy))
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered.origin = rec.point;
        scattered.direction = direction;
        return true;
    }

    return false;

}

Sphere unpack_sphere(int index) {
    vec4 cr = inSpheres[index].center_radius;
    vec4 cm = inSpheres[index].color_matId;

    int matId = int(cm.w + 0.5);

    // Unpack material
    vec4 af = inMaterials[matId].albedo_fuzz;
    vec4 ti = inMaterials[matId].type_ref_pad;

    Material m;
    m.type             = int(ti.x + 0.5);
    m.albedo           = af.rgb;
    m.refraction_index = ti.y;
    m.fuzz             = af.w;

    // Build sphere
    Sphere s;
    s.sphereCenter = cr.xyz;
    s.sphereRadius = cr.w;
    s.mat          = m;
    return s;
}

// put in sphere.glsl
bool intersectSphere(Ray r, inout Interval ray_t, out hit_record rec, int index) {

        // Save time by unpacking radius/center only and no materials
        vec3 unpacked_center = inSpheres[index].center_radius.xyz;
        float unpacked_radius = inSpheres[index].center_radius.w;
        
        vec3 oc = unpacked_center - r.origin;
        float a = dot(r.direction, r.direction);
        float h = dot(r.direction, oc);
        float c = dot(oc, oc) - unpacked_radius*unpacked_radius;

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

        Sphere obj = unpack_sphere(index);

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

        for (int i = 0; i < uSphereCount; ++i) {
            hit_record temp_rec;

            // Find closest hit, save it to closest_rec
            Interval testT = Interval(0.001, closest_hit);
             
 
            if (intersectSphere(r, testT, temp_rec, i)) {
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

void main() {

    // Get the pixel coordinates for this thread ( use global invocation ID )
    pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 resolution = ivec2(SCR_WIDTH, SCR_HEIGHT);

    // Stop if we're outside the screen dimensions
    if (pixel_coords.x >= resolution.x || pixel_coords.y >= resolution.y) {
        return;
    }

    vec3 pixel_color = vec3(0.0);
    iSeed = uSeed;

    // iSeed (iteration seed), unique for each sample, init from uniform uSeed
    iSeed = uSeed;

    for (int i = 0; i < SAMPLES; ++i) {

        //Update iteration seed
        iSeed += i;

        // 1) Create a seed for jitter vec generation
        float fi = float(i);
        vec2 seed = pixel_coords.xy + vec2(iSeed + fi, iSeed - fi);

        // 2) Create jitter offset for anti-aliasing [-0.5, 0.5]
        vec2 jitter = vec2(
            random_float(seed + vec2(1.0,0.0)),
            random_float(seed + vec2(0.0,1.0))
        ) - 0.5;

        // 3) Set pixel coords with offset for current sample
        vec2 pixel_coords = vec2(pixel_coords.xy) + jitter;

        // 4) Convert pixel coords to UV coords
        vec2 uv = pixel_coords / vec2(resolution);


        vec3 filmPoint = update_camera(cam, uv, resolution);
        vec3 dir       = normalize(filmPoint - cam.lookfrom);
        Ray  r         = make_ray(cam, cam.lookfrom, dir, filmPoint);

        pixel_color += ray_color(r);
    }


    // 6) average
    pixel_color /= float(SAMPLES);



    vec3 gammaCorrected = pow(pixel_color, vec3(1.0/2.2));


    // Write the result to the SSBO. 
    imageStore(imgOutput, pixel_coords, vec4(pixel_color, 1.0));

}

