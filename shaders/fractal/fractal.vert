#version 450

layout(binding = 0) uniform UniformBuffer {
    vec2 pos;
    vec2 ssize;
    float zoom;
} ub;



layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;

layout(location = 0) out vec2 fragUV;



void main() {
    gl_Position = vec4(inPosition, 1.0f, 1.0f);

    vec2 corrected = vec2(inTexCoords.x, inTexCoords.y * (ub.ssize.y / ub.ssize.x));
    fragUV = (corrected * ub.zoom) - ub.pos;
}

