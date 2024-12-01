#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;

void main() {
    gl_Position = vec4(inPosition, 1.0f, 1.0f);
}

