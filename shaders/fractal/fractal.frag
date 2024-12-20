#version 450

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 outColor;


void main() {

    vec2 z = vec2(0.0f, 0.0f);
    int iters = 0;
    int total_iters = 10000;
    for (int i = 0; i < total_iters; i++) {
        z = vec2(z.x * z.x - z.y * z.y, 2 * z.x * z.y);
        z += fragUV;
        if (length(z) > 1000) {
            break;
        }
        iters++;
    }

    outColor = vec4(0.0f);
    if (iters < total_iters) {
        float it = float(iters) + 1 - log(log(length(z)))/log(2);
        float b = floor(it);
        outColor = vec4(it/50.0) * vec4(0.7f, 0.9f, 1.0f, 1.0f);
    }
}
