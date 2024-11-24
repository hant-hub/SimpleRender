#version 450


layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 uv;

layout (location = 0) out vec2 fragUV;

layout(binding = 0) uniform UniformBuffer {
    mat4 proj;
} ub;

void main() {
    gl_Position = ub.proj * vec4(inPosition, 1.0f, 1.5f);
    fragUV = uv;
}

