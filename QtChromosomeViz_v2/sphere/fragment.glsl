#version 330 core

uniform vec3 eye;
uniform vec3 cx, cy, cz;
uniform float ufFocalLength;

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
    vec3 cvLightDirection = cz;
    vec4 baseColor = cColor;
    
    vec2 vScreenPos = 0.5f * (vPosition.xy * uvScreenSize) / vPosition.w;
    
    // Normal
    vec3 D = vScreenPos.x * cx + vScreenPos.y * cy - ufFocalLength * cz;
    vec3 V = vInstancePosition - eye;
    
    float DV = dot(D, V);
    float D2 = dot(D, D);
    float SQ = DV*DV-D2*(dot(V, V)-fInstanceSize*fInstanceSize);
    if(SQ < 0) discard;
    SQ = sqrt(SQ);
    float T1 = (DV+SQ)/D2;
    float T2 = (DV-SQ)/D2;
    float Result = (T1<T2 ? T1 : T2);
    
    vec3 IP = eye+Result*D;
    vec3 Normal = (IP-vInstancePosition)/fInstanceSize;
    
    V = eye-IP;
    vec3 Refl = reflect(-cvLightDirection,Normal);
    
    vec3 Pixel = vec3(0, 0, 0);// Ambient light
    
    // Diffuse:
    float Factor = dot(Normal, cvLightDirection);
    if (Factor>0)
        Pixel += vec3(1, 1, 1)*baseColor.rgb*Factor;
    
    // Specular:
    float pa = 0;
    float ps = 40;
    Factor = dot(normalize(Refl),cvLightDirection);
    if (Factor > 0)
        Pixel += vec3(1, 1, 1)*pow(Factor, ps)*pa;
    
    ocColor = vec4(Pixel, 1);
    /*// Fog
    float linearDistance = length(vViewPosition.rgb);
    float fogFactor = mix(1.0,
                          clamp(exp(-ufFogDensity * linearDistance), 0.0, 1.0),
                          ufFogContribution);

    // Calculate stripes for selected molecules
    float stripePhase = 0.5f * (vScreenPos.x + vScreenPos.y);
    float whitening = clamp(0.5f * (3.f * sin(stripePhase)), 0.f, 0.666f);

    float isSelected = ((iFlags & 4u) == 4u) ? 1.f : 0.f;
    vec4 cResultColor = vec4(mix(ucFogColor, cDiffuse.rgb + cSpecular.rgb, fogFactor), baseColor.a);
    ocColor = mix(cResultColor, vec4(1.f, 1.f, 1.f, 1.f), isSelected * whitening);*/
}
