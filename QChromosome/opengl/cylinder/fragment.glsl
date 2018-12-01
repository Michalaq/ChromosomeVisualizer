#version 410 core

layout (std140) uniform shader_data
{
    mat4 pro;
    int pro_flagBits; 
    mat4 mv;
    int mv_flagBits;
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
};

in vec4 vViewPosition;
in vec3 vNormal;
flat in int iFlags;
in vec4 cColor;
in vec3 cSpecularColor;
in float fSpecularExponent;

out vec4 fragColor;

void main() {
    const vec3 cvLightDirection = normalize(vec3(-1., 1., 2.));
    vec3 vFixedNormal = normalize(vNormal);
    vec4 baseColor = cColor;
    
    ivec2 iScreenPos = ivec2(gl_FragCoord.xy) & 1;
    
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

    // Diffuse
    float lightness = 0.5 + 0.5 * dot(cvLightDirection, vFixedNormal);
    vec3 cDiffuse = vec3(baseColor.rgb * lightness);

    // Specular
    vec3 reflected = reflect(-cvLightDirection, vFixedNormal);
    float specularFactor = pow(max(0.0, reflected.z), fSpecularExponent);
    vec3 cSpecular = specularFactor * cSpecularColor;

    // Fog
    float linearDistance = length(vViewPosition.xyz);
    float fogFactor = exp(-linearDistance / ufFogDistance);
    if (!ubEnableFog) fogFactor = 1.f;
    
    fragColor = vec4(mix(cDiffuse + cSpecular, unpackUnorm4x8(ucFogColor).bgr, ufFogStrength * (1.f - fogFactor)), 1.f);
}

