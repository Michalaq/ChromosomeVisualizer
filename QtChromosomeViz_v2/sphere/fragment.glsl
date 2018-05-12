#version 420 core

layout (std140) uniform shader_data
{
    mat4 pro;
    int pro_flagBits; 
    mat4 mv;
    int mv_flagBits;
    ivec2 uvScreenSize;
};

layout (std140) uniform viewport_data
{
    uint ucBackgroundColor;
    uint ucEnvironmentColor;
    float ufEnvironmentStrength;
    bool ubEnableFog;
    uint ucFogColor;
    float ufFogStrength;
    float ufFogDistance;
    bool ubAffectBackground;
};

struct Material
{
    uint cColor;
    uint cSpecularColor;
    float fSpecularExponent;
};

layout (std140) uniform material_data
{
    Material materials[50];
};

in vec4 vPosition;
in vec3 vViewPosition;

flat in vec3 vInstancePosition;
flat in int iFlags;
flat in float fInstanceSize;
flat in int iMaterialID;

out vec4 fragColor;

void main() {
    const vec3 cvLightDirection = normalize(vec3(-1., 1., 2.));
    vec4 baseColor = unpackUnorm4x8(materials[iMaterialID].cColor).bgra;
    
    vec2 vScreenPos = (0.5f * vPosition.xy / vPosition.w + 0.5f) * uvScreenSize;
    ivec2 iScreenPos = ivec2(vScreenPos) & 1;
    
    if (baseColor.a <= 0.51)
    {
        if (iScreenPos.x == 0 && iScreenPos.y == 1)
            discard;
    }
    
    if (baseColor.a <= 0.33)
    {
        if (iScreenPos.x == 1 && iScreenPos.y == 0)
            discard;
    }
    
    if (baseColor.a <= 0.20)
    {
        if (iScreenPos.x == 1 && iScreenPos.y == 1)
            discard;
    }
    
    baseColor.a = 1.0;
    
    // Normal
    float p = dot(vViewPosition, vViewPosition);
    float q = dot(vViewPosition, vInstancePosition);
    float r = dot(vInstancePosition, vInstancePosition);

    float d = q * q - p * (r - fInstanceSize * fInstanceSize);
    
    if (d < 0.0)
        discard;
    
    d = sqrt(d);
    
    float s = sign(q - d);
    float t = (q - s * d) / p;
    
    vec3 vNormal = s * (t * vViewPosition - vInstancePosition) / fInstanceSize;
    vec4 vFragCoord = pro * vec4(t * vViewPosition, 1.0);
    
    gl_FragDepth = 0.5 * vFragCoord.z / vFragCoord.w + 0.5;

    // Diffuse
    float lightness = 0.5 + 0.5 * dot(cvLightDirection, vNormal);
    vec4 cDiffuse = vec4(baseColor.xyz * lightness, baseColor.a);

    // Specular
    vec3 reflected = reflect(-cvLightDirection, vNormal);
    float specularFactor = pow(max(0.0, reflected.z), materials[iMaterialID].fSpecularExponent);
    vec4 cSpecular = vec4(specularFactor * unpackUnorm4x8(materials[iMaterialID].cSpecularColor).bgr, 0.0);

    // Fog
    float linearDistance = length(vViewPosition.xyz);
    float fogFactor = min(linearDistance / ufFogDistance, 1.f);
    if (!ubEnableFog) fogFactor = 0.f;

    // Calculate stripes for selected molecules
    float stripePhase = 0.5f * (vScreenPos.x + vScreenPos.y);
    float whitening = clamp(0.5f * (3.f * sin(stripePhase)), 0.f, 0.666f);

    float isSelected = ((iFlags & 0x1) == 0x1) ? 1.f : 0.f;
    vec4 cResultColor = vec4(mix(cDiffuse.rgb + cSpecular.rgb, unpackUnorm4x8(ucFogColor).bgr * ufFogStrength, fogFactor), baseColor.a);
    
    fragColor = mix(cResultColor, vec4(1.f, 1.f, 1.f, 1.f), isSelected * whitening);
}
