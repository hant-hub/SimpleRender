#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;

layout(location = 0) out vec2 fragUV;

layout(binding = 0) uniform Uniform {
    mat4 proj;
    mat4 view;
    vec2 tilesize;
    vec2 imgsize;
} ub;


void main() {
    gl_Position = ub.proj * ub.view * vec4(inPosition, 1.0f, 1.0f);
    fragUV = ub.tilesize * inTexCoords;
}

