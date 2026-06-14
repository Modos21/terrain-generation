#pragma once

struct Material {
    vec3 ambient;
    vec4 diffuse;
    vec3 specular;
    float shininess;
};

/*
 * Calculates the diffuse light part of Phong lighting for a fragment in View Space
 */
vec3 diffuseVS(in vec3 lightDir, in vec3 fragNormal_vs, in vec3 diffuseColor) {
    float diffFac = max(dot(fragNormal_vs, lightDir), 0.0);
    return diffuseColor * diffFac;
}

/*
 * Calculates the specular light part of Phong lighting for a fragment in View Space
 */
vec3 specularVS(in vec3 reflectedLightDir, in vec3 fragPos_vs, in vec3 specularColor, in float shininess) {
    vec3 viewDir = normalize(-fragPos_vs);
    float specFac = pow(max(dot(viewDir, reflectedLightDir), 0.0), shininess);
    return specularColor * specFac;
}

/*
 * Calculates Phong lighting for a fragment in View Space
 */
vec3 phongVS(in vec3 lightPos_vs, in vec3 fragPos_vs, in vec3 fragNormal_vs, in Material mat) {
    vec3 lightDir = normalize(lightPos_vs - fragPos_vs);
    vec3 reflectedLightDir = normalize(reflect(-lightDir, fragNormal_vs));

    vec3 amb = mat.ambient;
    vec3 diff = diffuseVS(lightDir, fragNormal_vs, vec3(mat.diffuse.rgb));
    vec3 spec = specularVS(reflectedLightDir, fragPos_vs, mat.specular, mat.shininess);

    return amb + diff + spec;
}