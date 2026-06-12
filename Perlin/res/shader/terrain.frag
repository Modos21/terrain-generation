#version 460 core

#define AIR 0u
#define GRASS 1u
#define DIRT 2u
#define STONE 3u
#define WATER 4u
#define LAVA 5u

in vec3 vNormal;
in vec2 vUV;
flat in uint vType;

out vec4 fragColor;

vec4 blockColor(uint type) {
    vec4 color  = vec4(1.0, 0.0, 1.0, 1);
    switch (type) {
        case GRASS: color = vec4(0.27, 0.54, 0.15, 1.00); break;
        case DIRT : color = vec4(0.48, 0.31, 0.18, 1.00); break;
        case STONE: color = vec4(0.50, 0.50, 0.50, 1.00); break;
        case WATER: color = vec4(0.00, 0.00, 0.90, 0.70); break;
        case LAVA : color = vec4(0.90, 0.50, 0.00, 1.00); break;
        default: break;
    }
    color.a = 1 - color.a;
    return color;
}

void main() {
    vec4 color = blockColor(vType);

    // Einfaches Directional Light
    vec3 lightDir = normalize(vec3(1.0, 2.0, 1.0));
    float diff    = (vType == WATER) ? 1.0 : max(dot(normalize(vNormal), lightDir), 0.0);
    float ambient = 0.35;
    color *= (ambient + diff * 0.65);

    fragColor = color;
}