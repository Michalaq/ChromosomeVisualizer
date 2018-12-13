#version 410 core

uniform vec3 uvDefaultLight;

in vec2 glPosition;

out vec4 fragColor;

void main() {
    float r = dot(glPosition, glPosition);
    if (r > 1.0) discard;
    
    // Normal
    vec3 vNormal = vec3(glPosition, sqrt(1.0 - r));

    // Diffuse
    float lightness = dot(uvDefaultLight, vNormal);
    vec3 cDiffuse = vec3(1.0, 1.0, 1.0) * lightness;
    
    fragColor = vec4(cDiffuse, 1.0);
}
