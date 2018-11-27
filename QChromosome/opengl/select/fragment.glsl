#version 410 core

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
flat in float fInstanceSize;

out float fragDepth;

void main() {
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
    
    vec4 vFragCoord = pro * vec4(t * vViewPosition, 1.0);
    
    gl_FragDepth = fragDepth = 0.5 * vFragCoord.z / vFragCoord.w + 0.5;
}
