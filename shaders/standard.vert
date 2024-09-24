#version 450

struct Uniform {
    mat4 model;
    uint texIndex;
};


layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out uint index;

layout(binding = 0) uniform UniformBuffer {
    mat4 view;
    mat4 proj;
    Uniform Modeldata[100];
} ub;




void main() {
    gl_Position = ub.proj * ub.view * ub.Modeldata[gl_InstanceIndex].model * vec4(inPosition, 1.0f, 1.0f);

    fragUV = inTexCoords;
    index = ub.Modeldata[gl_InstanceIndex].texIndex;
}

