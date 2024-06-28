#version 410 core

// output shader variable
out vec4 colorFrag;

// interpolated texture coordinates
in vec2 interp_UV;

// texture repetitions
uniform float repeat;

// texture sampler
uniform sampler2D tex;

// main function
void main(void)
{
    // we repeat the UVs and we sample the texture
    vec2 repeated_UV = mod(interp_UV * repeat, 1.0);
    vec4 surfaceColor = texture(tex, repeated_UV);

    // set the final fragment color
    colorFrag = surfaceColor;
}