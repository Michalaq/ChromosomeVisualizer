#version 330 core

uniform vec2 uvScreenSize;
uniform vec3 ucSpecularColor;
uniform float ufSpecularExponent;

in vec4 vPosition;
in vec3 vNormal;
flat in uint iFlags;
in vec4 vColor;

out vec4 cColor;

void main() {
    const vec3 cvLightDirection = vec3(0.7, 0.0, 0.7);
    vec3 vFixedNormal = normalize(vNormal);
    vec4 baseColor = vColor;

    // Diffuse
    float lightness = 0.5 + 0.5 * dot(cvLightDirection, vFixedNormal);
    vec4 cDiffuse = vec4(baseColor.xyz * lightness, baseColor.a);

    // Specular
    vec3 reflected = reflect(-cvLightDirection, vFixedNormal);
    float specularFactor = pow(max(0.0, reflected.z), ufSpecularExponent);
    vec4 cSpecular = vec4(specularFactor * ucSpecularColor, 0.0);

    // Calculate stripes for selected molecules
    vec2 vScreenPos = 0.5f * (vPosition.xy * uvScreenSize) / vPosition.w;
    float stripePhase = 0.5f * (vScreenPos.x + vScreenPos.y);
    float whitening = clamp(0.5f * (3.f * sin(stripePhase)), 0.f, 0.666f);

    float isSelected = ((iFlags & 4u) == 4u) ? 1.f : 0.f;
    cColor = mix(cDiffuse + cSpecular, vec4(1.f, 1.f, 1.f, 1.f), isSelected * whitening);
}
