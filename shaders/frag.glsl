#version 430 core

in  vec2 fragUV;
out vec4 FragColor;

struct Ray {
    vec3 origin;
    vec3 direction;
};

bool hit_sphere(vec3 center, float radius, Ray r) {
    vec3 oc = center - r.origin;
    float a = dot(r.direction, r.direction);
    float b = -2.0 * dot(r.direction, oc);
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - 4*a*c;
    return (discriminant >= 0);
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

vec3 ray_color(Ray r) {

    if (hit_sphere(vec3(0,0,-1), 0.5, r))
        return vec3(1, 0, 0);

    // simple gradient background
    vec3 unit_dir = normalize(r.direction);
    float t = 0.5 * (unit_dir.y + 1.0);
    return mix(
        vec3(1.0, 1.0, 1.0),
        vec3(0.5, 0.7, 1.0),
        t
    );
}

void main() {

    // AHard code resolution
    ivec2 resolution = ivec2(1280, 720);
    vec3 cam_origin = vec3(0.0);

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
