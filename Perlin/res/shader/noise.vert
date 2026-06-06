#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex;

uniform mat4 u_mvpMatrix;

uniform bool u_doDisplacement;
uniform sampler2D u_noiseTex;
uniform float u_heightScale;
uniform vec2 u_planeQuadSize;

out vec2 texCoord;
out vec3 pos_ws;
out vec3 norm_ws;

/**
 * Wendet alle Transformationen (Wind, Heightmap) auf die Position und Normale an.
 */
void main(void) {
    pos_ws = pos;
    norm_ws = normal;
    texCoord = tex;

    if (u_doDisplacement) {
        pos_ws += normal * texture(u_noiseTex, tex).r * u_heightScale;
    }

    gl_Position = u_mvpMatrix * vec4(pos_ws, 1.0);
}
