
#include "/camera.glsl_h"

vec3 update_camera(in Camera cam, vec2 uv, vec2 res){

    // Initialize the camera

    vec3 camDir = normalize(cam.lookfrom - cam.lookat);
    vec3 camRight = normalize(cross(cam.vup, camDir));
    vec3 camUp = cross(camDir, camRight); // What is considered up relative to cam


    float aspect      = float(res.x) / float(res.y);
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

