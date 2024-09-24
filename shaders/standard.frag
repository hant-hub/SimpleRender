#version 450

layout(location = 0) in vec2 fragUV;
layout (location = 1) in flat uint index;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D texsampler[2];



void main() {
    switch(index) {
        case 0:
            outColor = texture(texsampler[0], fragUV);
            outColor = vec4(vec3((0.2126 * outColor.x + 0.7152 * outColor.y + 0.0722 * outColor.z)), 1.0f);
            break;
        case 1:
            outColor = texture(texsampler[1], fragUV);
            outColor = vec4(vec3((0.2126 * outColor.x + 0.7152 * outColor.y + 0.0722 * outColor.z)), 1.0f);
            outColor = outColor * vec4(1.0f, 0.1f, 0.1f, 1.0f);
            break;
        case 2:
            outColor = texture(texsampler[0], fragUV);
            outColor = vec4(vec3((0.2126 * outColor.x + 0.7152 * outColor.y + 0.0722 * outColor.z)), 1.0f);
            outColor = outColor * vec4(0.1f, 1.0f, 0.1f, 1.0f);
            break;
        case 3:
            outColor = texture(texsampler[1], fragUV);
            outColor = vec4(vec3((0.2126 * outColor.x + 0.7152 * outColor.y + 0.0722 * outColor.z)), 1.0f);
            outColor = outColor * vec4(0.1f, 0.1f, 1.0f, 1.0f);
            break;
    }
}
