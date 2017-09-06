#version 330 core

uniform vec3 eye;
uniform vec3 cx, cy, cz;

uniform vec2 uvScreenSize;
uniform float ufFogDensity;
uniform float ufFogContribution;
uniform vec3 ucFogColor;

in vec4 vPosition;
in vec4 vViewPosition;
flat in vec3 vInstancePosition;
flat in uint iFlags;
flat in vec4 cColor;
flat in vec3 cSpecularColor;
flat in float fSpecularExponent;
flat in float fInstanceSize;

out vec4 ocColor;

void main() {
    const vec3 cvLightDirection = normalize(vec3(-1., 1., 2.));
    vec4 baseColor = cColor;
    
    vec2 vScreenPos = 0.5f * (vPosition.xy * uvScreenSize) / vPosition.w;
    
    // Normal
    vec3 D = vScreenPos.x * cx + vScreenPos.y * cy - cz;
    vec3 V = vInstancePosition - eye;
    
    float DV = dot(D, V);
    float D2 = dot(D, D);
    float SQ = DV*DV-D2*(dot(V, V)-fInstanceSize*fInstanceSize);
    if(SQ < 0) discard;
    SQ = sqrt(SQ);
    float T1 = (DV+SQ)/D2;
    float T2 = (DV-SQ)/D2;
    float Result = (T1<T2 ? T1 : T2);

    vec3 vFixedNormal = normalize(Result*D - V);

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
    float stripePhase = 0.5f * (vScreenPos.x + vScreenPos.y);
    float whitening = clamp(0.5f * (3.f * sin(stripePhase)), 0.f, 0.666f);

    float isSelected = ((iFlags & 4u) == 4u) ? 1.f : 0.f;
    vec4 cResultColor = vec4(mix(ucFogColor, cDiffuse.rgb + cSpecular.rgb, fogFactor), baseColor.a);
    ocColor = mix(cResultColor, vec4(1.f, 1.f, 1.f, 1.f), isSelected * whitening);
}
