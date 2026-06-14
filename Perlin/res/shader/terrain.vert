#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNorm;
layout(location = 2) in vec2 aUV;
layout(location = 3) in uint aType;

uniform mat4 u_mvpMatrix;

out vec3 vNormal_ms;
out vec2 vUV;
flat out uint vType;

void main() {
    gl_Position = u_mvpMatrix * vec4(aPos, 1.0);
    vNormal_ms = aNorm;
    vUV = aUV;
    vType = aType;
}