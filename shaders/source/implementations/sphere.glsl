#ifndef SPHERE_GLSL
#define SPHERE_GLSL

#include "/sphere.glsl_h"
#include "/ray.glsl_h"

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


void set_face_normal(in Ray r, inout hit_record rec, in vec3 outward_normal) {
    // Sets the hit record normal vector.
    // NOTE: the parameter `outward_normal` is assumed to have unit length.

    // Sphere
    rec.front_face = dot(r.direction, outward_normal) < 0;
    rec.normal = rec.front_face ? outward_normal : -outward_normal;
}

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

#endif // Must end in newline
