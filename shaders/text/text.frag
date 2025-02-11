#version 450

layout (location = 0) in vec2 fragUV;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D texsampler;

layout(binding = 2) uniform UniformBuffer {
    vec3 color;
}ub;

void main() {
 //   outColor = vec4(1.0f);
    outColor = vec4(texture(texsampler, fragUV).r) * vec4(ub.color, 1.0f);
}
