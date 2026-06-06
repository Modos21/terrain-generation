#version 460 core

in vec2 texCoord;

uniform sampler2D u_noiseTex;

out vec4 fragColor;

/**
 * Gibt dem Fragment die definierte Farbe, oder zeichnet fbm.
 */
void main(void) {
    fragColor = texture(u_noiseTex, texCoord).y * vec4(1);
    fragColor.a = 1;
}
