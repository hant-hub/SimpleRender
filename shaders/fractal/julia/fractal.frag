#version 450

layout(binding = 0) uniform UniformBuffer {
    vec2 pos;
    vec2 size;
    vec3 color;
    float zoom;
    vec2 initial;
} ub;


layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 outColor;


void main() {

    vec2 z = fragUV;
    int iters = 0;
    int total_iters = 100;
    for (int i = 0; i < total_iters; i++) {
        z = vec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y);
        z += ub.initial;
        if (length(z) > total_iters) {
            break;
        }
        iters++;
    }

    outColor = vec4(0.0f);
    if (iters < total_iters) {
        float it = float(iters) + 1 - log(log(length(z)))/log(2);
        float b = floor(it);
        outColor = vec4(it/50.0) * vec4(ub.color, 1.0f);
    }
}
