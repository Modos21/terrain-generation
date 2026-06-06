#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 2) in vec2 tex;

uniform mat4 u_mvpMatrix;

out vec2 texCoord;

void main() {
    texCoord = tex;
    gl_Position = u_mvpMatrix * vec4(pos, 1);
}