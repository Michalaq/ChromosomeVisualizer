#version 410 core

layout (std140) uniform shader_data
{
    mat4 pro;
    int pro_flagBits; 
    mat4 mv;
    int mv_flagBits;
    int flags;
    int ptype;
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
    vec3 uvDefaultLight;
};

struct Material
{
    uint cColor;
    uint cSpecularColor;
    float fSpecularExponent;
};

layout (std140) uniform material_data
{
    uvec4 materials[50];
};

in vec3 vViewPosition;

flat in vec3 vInstancePosition;
flat in int iFlags;
flat in float fInstanceSize;
flat in int iMaterialID;

out vec4 fragColor;

void main() {
    vec4 baseColor = unpackUnorm4x8(materials[iMaterialID].x).bgra;
    
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
    
    // Normal
    vec3 vNormal;
    vec4 vFragCoord;
    
    switch (ptype)
    {
    case 0:
        {
            float p = dot(vViewPosition, vViewPosition);
            float q = dot(vViewPosition, vInstancePosition);
            float r = dot(vInstancePosition, vInstancePosition);

            float d = q * q - p * (r - fInstanceSize * fInstanceSize);
            
            if (d < 0.0)
                discard;
            
            d = sqrt(d);
            
            float s = sign(q - d);
            float t = (q - s * d) / p;
            
            vNormal = s * (t * vViewPosition - vInstancePosition) / fInstanceSize;
            vFragCoord = pro * vec4(t * vViewPosition, 1.0);
        }
        break;
        
    case 1:
        {
            vec2 r = vInstancePosition.xy - vViewPosition.xy;
            float d = fInstanceSize * fInstanceSize - dot(r, r);
            
            if (d < 0.0)
                discard;
            
            vec3 t = vec3(vViewPosition.xy, vInstancePosition.z + sqrt(d));
            
            vNormal = (t - vInstancePosition) / fInstanceSize;
            vFragCoord = pro * vec4(t, 1.0);
        }
        break;
        
    default:
        discard;
    }
    
    gl_FragDepth = 0.5 * vFragCoord.z / vFragCoord.w + 0.5;
    
    // Diffuse
    float lightness = 0.5 + 0.5 * dot(uvDefaultLight, vNormal);
    vec3 cDiffuse = baseColor.rgb * lightness;

    // Specular
    vec3 reflected = reflect(-uvDefaultLight, vNormal);
    float specularFactor = pow(max(0.0, reflected.z), uintBitsToFloat(materials[iMaterialID].z));
    vec3 cSpecular = specularFactor * unpackUnorm4x8(materials[iMaterialID].y).bgr;

    // Fog
    float linearDistance = length(vViewPosition.xyz);
    float fogFactor = exp(-linearDistance / ufFogDistance);
    if (!ubEnableFog) fogFactor = 1.f;

    vec3 cResultColor = mix(cDiffuse + cSpecular, unpackUnorm4x8(ucFogColor).bgr, ufFogStrength * (1.f - fogFactor));

    fragColor = vec4(mix(cResultColor, vec3(1.f), ((iFlags & 0x1) == 0x1) ? .33 : 0.f), 1.f);
}
