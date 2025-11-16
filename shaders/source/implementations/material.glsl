

#include "/material.glsl_h"

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

