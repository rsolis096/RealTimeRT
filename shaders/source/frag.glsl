#version 430 core
in  vec2 fragUV;
out vec4 FragColor;

uniform sampler2D uOutputTexture;

void main() {

    FragColor = texture(uOutputTexture, fragUV);
}