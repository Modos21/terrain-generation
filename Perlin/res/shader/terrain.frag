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

vec3 blockColor(uint type) {
    switch (type) {
        case GRASS: return vec3(0.27, 0.54, 0.15);
        case DIRT : return vec3(0.48, 0.31, 0.18);
        case STONE: return vec3(0.50, 0.50, 0.50);
        case WATER: return vec3(0.00, 0.00, 0.90);
        case LAVA : return vec3(0.90, 0.50, 0.00);
        default: return vec3(1.0, 0.0, 1.0);
    }
}

void main() {
    vec3 color = blockColor(vType);

    // Einfaches Directional Light
    vec3 lightDir = normalize(vec3(1.0, 2.0, 1.0));
    float diff    = max(dot(normalize(vNormal), lightDir), 0.0);
    float ambient = 0.35;
    color *= (ambient + diff * 0.65);

    fragColor = vec4(color, 1.0);
}