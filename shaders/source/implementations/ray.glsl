

#include "/ray.glsl_h"

Ray make_ray(in Camera camera, vec3 origin, vec3 direction, vec3 filmPoint) {
    Ray r;
    r.origin    = (camera.defocus_angle <= 0) ? origin : defocus_disk_sample(origin);
    r.direction = normalize(filmPoint - r.origin);
    return r;
}

vec3 ray_at(Ray r, float t) {
    return r.origin + t * r.direction;
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

