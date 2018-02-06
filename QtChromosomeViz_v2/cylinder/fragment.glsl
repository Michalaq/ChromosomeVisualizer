#version 330 core

uniform vec2 uvScreenSize;
uniform float ufFogDensity;
uniform float ufFogContribution;
uniform vec3 ucFogColor;

in vec4 vPosition;
in vec4 vViewPosition;
in vec3 vNormal;
flat in int iFlags;
in vec4 cColor;
in vec3 cSpecularColor;
in float fSpecularExponent;

void main() {
    const vec3 cvLightDirection = normalize(vec3(-1., 1., 2.));
    vec3 vFixedNormal = normalize(vNormal);
    vec4 baseColor = cColor;

    // Diffuse
    float lightness = 0.5 + 0.5 * dot(cvLightDirection, vFixedNormal);
    vec4 cDiffuse = vec4(baseColor.xyz * lightness, baseColor.a);

    // Specular
    vec3 reflected = reflect(-cvLightDirection, vFixedNormal);
    float specularFactor = pow(max(0.0, reflected.z), fSpecularExponent);
    vec4 cSpecular = vec4(specularFactor * cSpecularColor, 0.0);

    // Fog
    float linearDistance = length(vViewPosition.rgb);
    float fogFactor = mix(1.0,
                          clamp(exp(-ufFogDensity * linearDistance), 0.0, 1.0),
                          ufFogContribution);

    // Calculate stripes for selected molecules
    vec2 vScreenPos = 0.5f * (vPosition.xy * uvScreenSize) / vPosition.w;
    float stripePhase = 0.5f * (vScreenPos.x + vScreenPos.y);
    float whitening = clamp(0.5f * (3.f * sin(stripePhase)), 0.f, 0.666f);

    float isSelected = ((iFlags & 0x1) == 0x1) ? 1.f : 0.f;
    vec4 cResultColor = vec4(mix(ucFogColor, cDiffuse.rgb + cSpecular.rgb, fogFactor), baseColor.a);
    
    gl_FragColor = mix(cResultColor, vec4(1.f, 1.f, 1.f, 1.f), isSelected * whitening);
}
