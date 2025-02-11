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


vec2 cmul (vec2 a, vec2 b) {
	return vec2(a.x * b.x - a.y * b.y, a.x * b.y + b.x * a.y);
}

void main() {

    vec2 z = ub.initial;
    int iters = 0;
    int total_iters = 300;
    int max_pow = 10;
    for (int i = 0; i < total_iters; i++) {
        z = cmul(z, z); 
        z += fragUV;
        if (length(z) > 1.1 * total_iters) {
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
    //outColor = vec4(ub.p/4.0);
}
