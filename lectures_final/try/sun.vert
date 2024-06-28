#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec2 interp_UV;

void main() {
    interp_UV = aTexCoords;

    vec4 mvPosition = viewMatrix * modelMatrix * vec4(aPos, 1.0);
    gl_Position = projectionMatrix * mvPosition;
}