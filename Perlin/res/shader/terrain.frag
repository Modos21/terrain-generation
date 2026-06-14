#version 460 core

#include "blocktypes.glsl"

in vec3 vNormal;
in vec2 vUV;
flat in uint vType;

uniform sampler2D u_texStone;
uniform sampler2D u_texGrass;
uniform sampler2D u_texGrassSide;
uniform sampler2D u_texDirt;
uniform sampler2D u_texSand;
uniform sampler2D u_texWater;
uniform sampler2D u_texLava;
uniform sampler2D u_texBedrock;
uniform sampler2D u_texIronOre;
uniform sampler2D u_texDiamondOre;

out vec4 fragColor;

vec4 blockTexture(uint type) {
    vec4 color  = vec4(1.0, 0.0, 1.0, 1);
    // stone and bedrock tex are grayscale, so we only need the R channel then copy it to all 3 color channels
    switch (type) {
        case GRASS      : color = vec4(texture(u_texGrass, vUV).rgb, 1.0); break;
        case GRASS_SIDE : color = vec4(texture(u_texGrassSide, vUV).rgb, 1.0); break;
        case DIRT       : color = vec4(texture(u_texDirt, vUV).rgb, 1.0); break;
        case STONE      : color = vec4(vec3(texture(u_texStone, vUV).r), 1.0); break;
        case WATER      : color = vec4(texture(u_texWater, vUV).rgb, 0.3); break;
        case LAVA       : color = vec4(texture(u_texLava, vUV).rgb, 1.0); break;
        case SAND       : color = vec4(texture(u_texSand, vUV).rgb, 1.0); break;
        case BEDROCK    : color = vec4(vec3(texture(u_texBedrock, vUV).r), 1.0); break;
        case IRON_ORE   : color = vec4(texture(u_texIronOre, vUV).rgb, 1.0); break;
        case DIAMOND_ORE: color = vec4(texture(u_texDiamondOre, vUV).rgb, 1.0); break;
        default: break;
    }
    return color;
}

void main() {
    vec4 color = blockTexture(vType);

    // Einfaches Directional Light
    vec3 lightDir = normalize(vec3(1.0, 2.0, 1.0));
    float diff    = (vType == WATER) ? 1.0 : max(dot(normalize(vNormal), lightDir), 0.0);
    float ambient = 0.35;
    color *= (ambient + diff * 0.65);

    fragColor = color;
}