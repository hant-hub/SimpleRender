#version 450

struct Uniform {
    mat4 model;
};


layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUV;
layout(location = 2) out int index;

layout(binding = 0) uniform UniformBuffer {
    mat4 view;
    mat4 proj;
    Uniform Modeldata[100];
} ub;




void main() {
    gl_Position = ub.proj * ub.view * ub.Modeldata[gl_InstanceIndex].model * vec4(inPosition, 1.0f, 1.0f);

    fragColor = vec3(0);
    fragUV = inTexCoords;
    index = gl_InstanceIndex;
}

