#version 430 core
out vec2 fragUV;

void main() {

    // Create a really big triangle that fills the screen (3 vertices)
    const vec2 pos[3] = vec2[3](
        vec2(-1.0, -1.0),  // bottomleft
        vec2( 3.0, -1.0),  // bottomright (offscreen)
        vec2(-1.0,  3.0)   // topleft   (offscreen)

    );

    // Map NDC [-1,1] to UV [0,1]
    fragUV = (pos[gl_VertexID] + 1.0) * 0.5;

    // Output clipspace position
    gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);
}