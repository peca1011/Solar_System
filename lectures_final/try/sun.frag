#version 410 core

const float PI = 3.14159265359;

out vec4 FragColor;

in vec2 interp_UV;

uniform float repeat;
uniform sampler2D tex;
uniform bool useTexture;

void main() {
    vec2 repeated_UV = mod(interp_UV * repeat, 1.0);
    vec4 surfaceColor = useTexture ? texture(tex, repeated_UV) : vec4(1.0);
    FragColor = surfaceColor;
}