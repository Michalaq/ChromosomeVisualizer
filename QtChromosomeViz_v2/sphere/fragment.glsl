#version 330 core

uniform vec3 eye;
uniform vec3 cx, cy, cz;

uniform mat4 mvp;
uniform mat3 mvNormal;
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
    
    // Normal
    mat4 mvp = transpose(mvp);
    
    vec4 a = vPosition.w * mvp[0] - vPosition.x * mvp[3];
    vec4 b = vPosition.w * mvp[1] - vPosition.y * mvp[3];
    
    vec3 c = cross(a.xyz, b.xyz);
    vec3 d = cross(a.xyw, b.xyw);
    
    d = vec3(d.xy, 0.) - c.z * vInstancePosition;
    
    float p = dot(c, c);
    float q = dot(c, d);
    float r = dot(d, d) - (c.z * fInstanceSize) * (c.z * fInstanceSize);
    
    float z = (sqrt(q * q - p * r) - q) / p;
    
    vec3 vNormal = normalize(c * z + d);
    vec3 vFixedNormal = mvNormal * vNormal;

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

    float isSelected = ((iFlags & 4u) == 4u) ? 1.f : 0.f;
    vec4 cResultColor = vec4(mix(ucFogColor, cDiffuse.rgb + cSpecular.rgb, fogFactor), baseColor.a);
    ocColor = mix(cResultColor, vec4(1.f, 1.f, 1.f, 1.f), isSelected * whitening);
}
