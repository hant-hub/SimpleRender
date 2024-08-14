#version 450

struct Uniform {
    mat4 model;
//    mat4 view;
 //   mat4 proj;
};


layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUV;

layout(binding = 0) uniform UniformBuffer {
    Uniform data[100];
} ub;

void main() {
    gl_Position = ub.data[gl_InstanceIndex].model * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
    fragUV = inTexCoords;
}

