#version 410 core

// number of lights in the scene
#define NR_LIGHTS 1

const float PI = 3.14159265359;

// output shader variable
out vec4 colorFrag;

// array with lights incidence directions (calculated in vertex shader, interpolated by rasterization)
in vec3 lightDirs[NR_LIGHTS];

// the transformed normal has been calculated per-vertex in the vertex shader
in vec3 vNormal;
// vector from fragment to camera (in view coordinate)
in vec3 vViewPosition;

in vec2 interp_UV;

uniform float repeat;
uniform sampler2D tex;
uniform bool useTexture; // Neues Uniform, um die Textur zu aktivieren/deaktivieren

// ambient, diffusive and specular components (passed from the application)
uniform vec3 ambientColor;
uniform vec3 specularColor;
// weight of the components
uniform float Ka;
uniform float Kd;
uniform float Ks;

// shininess coefficients (passed from the application)
uniform float shininess;

// uniforms for GGX model
uniform float alpha; // rugosity - 0 : smooth, 1: rough
uniform float F0; // fresnel reflectance at normal incidence

////////////////////////////////////////////////////////////////////

// the "type" of the Subroutine
subroutine vec4 ill_model();

// Subroutine Uniform (it is conceptually similar to a C pointer function)
subroutine uniform ill_model Illumination_Model_ML;

////////////////////////////////////////////////////////////////////

//////////////////////////////////////////
// a subroutine for the Blinn-Phong model for multiple lights
subroutine(ill_model)
vec4 BlinnPhong_ML() {
    vec2 repeated_UV = mod(interp_UV * repeat, 1.0);
    vec4 surfaceColor = useTexture ? texture(tex, repeated_UV) : vec4(1.0); // Falls keine Textur verwendet wird, setzen Sie die Farbe auf Weiß
    vec4 color = vec4(Ka * ambientColor, 1.0);
    vec3 N = normalize(vNormal);

    for (int i = 0; i < NR_LIGHTS; i++) {
        vec3 L = normalize(lightDirs[i]);
        float lambertian = max(dot(L, N), 0.0);

        if (lambertian > 0.0) {
            color += Kd * lambertian * surfaceColor;
        }
    }
    return color;
}
//////////////////////////////////////////

//////////////////////////////////////////
// Schlick-GGX method for geometry obstruction (used by GGX model)
float G1(float angle, float alpha)
{
    float r = (alpha + 1.0);
    float k = (r*r) / 8.0;

    float num   = angle;
    float denom = angle * (1.0 - k) + k;

    return num / denom;
}

//////////////////////////////////////////
// a subroutine for the GGX model for multiple lights
subroutine(ill_model)
vec4 GGX_ML() // this name is the one which is detected by the SetupShaders() function in the main application, and the one used to swap subroutines
{
    vec2 repeated_UV = mod(interp_UV*repeat,1.0);
    vec4 surfaceColor = useTexture ? texture(tex,repeated_UV) : vec4(1.0); // Falls keine Textur verwendet wird, setzen Sie die Farbe auf Weiß
    vec3 N = normalize(vNormal);

    vec3 lambert = (Kd*surfaceColor.rgb)/PI;
    vec3 color = vec3(0.0);

    for(int i = 0; i < NR_LIGHTS; i++)
    {
        vec3 L = normalize(lightDirs[i]);
        float NdotL = max(dot(N, L), 0.0);
        vec3 specular = vec3(0.0);

        if(NdotL > 0.0)
        {
            vec3 V = normalize( vViewPosition );
            vec3 H = normalize(L + V);

            float NdotH = max(dot(N, H), 0.0);
            float NdotV = max(dot(N, V), 0.0);
            float VdotH = max(dot(V, H), 0.0);
            float alpha_Squared = alpha * alpha;
            float NdotH_Squared = NdotH * NdotH;

            float G2 = G1(NdotV, alpha)*G1(NdotL, alpha);

            float D = alpha_Squared;
            float denom = (NdotH_Squared*(alpha_Squared-1.0)+1.0);
            D /= PI*denom*denom;

            vec3 F = vec3(pow(1.0 - VdotH, 5.0));
            F *= (1.0 - F0);
            F += F0;

            specular = (F * G2 * D) / (4.0 * NdotV * NdotL);

            color += (lambert + specular)*NdotL;
        }
    }
    return vec4(color,1.0);
}
//////////////////////////////////////////

// main
void main(void)
{
    // we call the pointer function Illumination_Model_ML():
    // the subroutine selected in the main application will be called and executed
    colorFrag = Illumination_Model_ML();
}