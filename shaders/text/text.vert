#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;
layout(location = 2) in int character;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out uint index;


void main() {
    gl_Position = vec4(inPosition, 1.0f, 1.0f);

    fragUV = inTexCoords;
    index = character;
}

