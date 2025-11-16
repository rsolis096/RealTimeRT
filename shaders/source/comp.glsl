#version 430 core
#extension GL_ARB_shading_language_include : require

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

// Output image texture, bound to image unit 0
layout(rgba32f, binding = 0) uniform image2D imgOutput;


/* Constants */
const int MAX_SPHERES = 100;
const int MAX_MATERIALS = 100;
const float POS_MAX = 3.402823466e+38;   // Max positive float
const float NEG_MAX = -3.402823466e+38;  // Max negative float
const float pi = 3.14159265358979323846;
vec3   defocus_disk_u;       // Disk X basis for defocus
vec3   defocus_disk_v;       // Disk Y basis for defocus
float iSeed;

/* Forward Uniforms */
uniform int MAX_DEPTH;
uniform int uSphereCount;
uniform int uMaterialsCount;

ivec2 pixel_coords; // replaces gl_fragcoords;

/* Includes (constants defined in main for now) */
#include "/utilities.glsl"
#include "/interval.glsl"
#include "/camera.glsl"
#include "/aabb.glsl"
#include "/sphere.glsl"
#include "/material.glsl"
#include "/ray.glsl"


/* Struct Definitions */

/* Uniforms */
uniform Camera cam;

uniform float uSeed; // Time used for random value seeding
uniform int SCR_WIDTH;
uniform int SCR_HEIGHT;
uniform int SAMPLES;

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

