#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 fragUV;

layout(binding = 0) uniform Uniform {
    mat4 proj;
    mat4 view;
    vec2 mapsize;
    vec2 tilesize;
    vec2 imgsize;
} ub;

layout(set = 0, binding = 2) uniform sampler2D texsampler;

layout(std430, set = 0, binding = 1) readonly buffer TileData {
   int data[]; 
} tiles;

void main() {
    
    int index = int(fragUV.y) * int(ub.mapsize.x) + int(fragUV.x);
    vec2 stride = vec2(1.0f)/ub.tilesize;
    index = tiles.data[index];
    vec2 offset = vec2(index % int(ub.tilesize.x), index / int(ub.tilesize.x)) * stride;

    vec2 scale = fract(fragUV) * stride;

    outColor = texture(texsampler, offset + scale);
    //outColor = vec4(fragUV.x, fragUV.y, 0.0f, 1.0f);
}
