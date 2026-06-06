#version 460 core

in vec2 texCoord;
in vec3 pos_ws;
in vec3 norm_ws;

uniform sampler2D u_noiseTex;
uniform vec4 u_planeColor = vec4(1);
uniform bool u_doDisplacement;

out vec4 fragColor;

/**
 * Gibt dem Fragment die definierte Farbe, oder zeichnet fbm.
 */
void main(void) {
    if (u_doDisplacement) {
        fragColor = vec4(vec3(pos_ws.y), 1);
    } else {
        fragColor = texture(u_noiseTex, texCoord).y * u_planeColor;
        fragColor.a = 1;
    }
}
