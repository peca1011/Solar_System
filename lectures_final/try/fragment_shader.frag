#version 410 core
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D tex;
uniform vec3 lightPos; // Position of the first light source
uniform vec3 lightPos2; // Position of the second light source
uniform vec3 viewPos;
uniform vec3 lightColor; // Color of the first light source
uniform vec3 lightColor2; // Color of the second light source
uniform vec3 objectColor;

out vec4 FragColor;

void main() {
    // Ambient for both lights
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    vec3 ambient2 = ambientStrength * lightColor2;

    // Diffuse for the first light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular for the first light
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Diffuse for the second light
    vec3 lightDir2 = normalize(lightPos2 - FragPos);
    float diff2 = max(dot(norm, lightDir2), 0.0);
    vec3 diffuse2 = diff2 * lightColor2;

    // Specular for the second light
    vec3 reflectDir2 = reflect(-lightDir2, norm);
    float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), 32);
    vec3 specular2 = specularStrength * spec2 * lightColor2;

    // Combine results
    vec3 ambient = ambient + ambient2;
    vec3 diffuse = diffuse + diffuse2;
    vec3 specular = specular + specular2;

    vec3 lighting = (ambient + diffuse + specular) * objectColor;
    vec4 textureColor = texture(tex, TexCoords);

    FragColor = vec4(lighting, 1.0) * textureColor;
}