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

float cabs (vec2 z) {
	return length(z);
}

vec2 cmul (vec2 a, vec2 b) {
	return vec2(a.x * b.x - a.y * b.y, a.x * b.y + b.x * a.y);
}

vec2 cexp (vec2 z) {
	return exp(z.x) * vec2(cos(z.y), sin(z.y));
}

// principal value of logarithm of z
vec2 clog (vec2 z) {
	return vec2(log(cabs(z)), atan(z.y, z.x));
}

// principal value of z to the a power
vec2 cpow (vec2 z, vec2 a) {
	return cexp(cmul(a, clog(z)));
}

void main() {

    vec2 z = vec2(1);
    int iters = 0;
    int total_iters = 100;
    for (int i = 0; i < total_iters; i++) {
        z = cpow(z, fragUV);
        z -= ub.initial;
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
}
