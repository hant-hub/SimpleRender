#version 450

struct Uniform {
    mat4 model;
    mat4 uv;
    uint texIndex;
};


layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out uint index;

layout(binding = 0) uniform UniformBuffer {
    mat4 view;
    mat4 proj;
} ub;

layout(std140, set = 0, binding = 1) readonly buffer ModelBuffer {
    Uniform ModelData[];
} models;




void main() {
    gl_Position = ub.proj * 
                  ub.view * 
                  models.ModelData[gl_InstanceIndex].model * 
                  vec4(inPosition, 1.0f, 1.0f);

    fragUV = (models.ModelData[gl_InstanceIndex].uv * vec4(inTexCoords, 1.0f, 1.0f)).xy;
    index = models.ModelData[gl_InstanceIndex].texIndex;
}

