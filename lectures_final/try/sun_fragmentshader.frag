#version 410 core

#define NR_LIGHTS 1

const float PI = 3.14159265359;

out vec4 FragColor;

in vec3 lightDirs[NR_LIGHTS];
in vec3 vNormal;
in vec3 vViewPosition;
in vec2 interp_UV;

uniform float repeat;
uniform sampler2D tex;
uniform bool useTexture;
uniform vec3 ambientColor;
uniform vec3 specularColor;
uniform float Ka;
uniform float Kd;
uniform float Ks;
uniform float shininess;
uniform float alpha;
uniform float F0;

subroutine vec4 ill_model();
subroutine uniform ill_model Illumination_Model_ML;

subroutine(ill_model)
vec4 BlinnPhong_ML() {
    vec2 repeated_UV = mod(interp_UV * repeat, 1.0);
    vec4 surfaceColor = useTexture ? texture(tex, repeated_UV) : vec4(1.0);
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

float G1(float angle, float alpha) {
    float r = (alpha + 1.0);
    float k = (r * r) / 8.0;

    float num = angle;
    float denom = angle * (1.0 - k) + k;

    return num / denom;
}

subroutine(ill_model)
vec4 GGX_ML() {
    vec2 repeated_UV = mod(interp_UV * repeat, 1.0);
    vec4 surfaceColor = useTexture ? texture(tex, repeated_UV) : vec4(1.0);
    vec3 N = normalize(vNormal);

    vec3 lambert = (Kd * surfaceColor.rgb) / PI;
    vec3 color = vec3(0.0);

    for (int i = 0; i < NR_LIGHTS; i++) {
        vec3 L = normalize(lightDirs[i]);
        float NdotL = max(dot(N, L), 0.0);
        vec3 specular = vec3(0.0);

        if (NdotL > 0.0) {
            vec3 V = normalize(vViewPosition);
            vec3 H = normalize(L + V);

            float NdotH = max(dot(N, H), 0.0);
            float NdotV = max(dot(N, V), 0.0);
            float VdotH = max(dot(V, H), 0.0);
            float alpha_Squared = alpha * alpha;
            float NdotH_Squared = NdotH * NdotH;

            float G2 = G1(NdotV, alpha) * G1(NdotL, alpha);

            float D = alpha_Squared;
            float denom = (NdotH_Squared * (alpha_Squared - 1.0) + 1.0);
            D /= PI * denom * denom;

            vec3 F = vec3(pow(1.0 - VdotH, 5.0));
            F *= (1.0 - F0);
            F += F0;

            specular = (F * G2 * D) / (4.0 * NdotV * NdotL);

            color += (lambert + specular) * NdotL;
        }
    }
    return vec4(color, 1.0);
}

void main() {
    FragColor = Illumination_Model_ML();
}