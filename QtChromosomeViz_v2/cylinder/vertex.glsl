#version 420 core

layout(location = 0) in vec3 vInstancePosition;
layout(location = 1) in int iInstanceFlags;
layout(location = 2) in float fInstanceSize;
layout(location = 3) in int iMaterialID;

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

flat out vec3 gvInstancePosition;
flat out int giFlags;
flat out vec4 gcColor;
flat out vec3 gcSpecularColor;
flat out float gfSpecularExponent;
flat out float gfInstanceSize;

void main() {
    gvInstancePosition = vInstancePosition;
    giFlags = iInstanceFlags;
    gcColor = unpackUnorm4x8(materials[iMaterialID].cColor).bgra;
    gcSpecularColor = unpackUnorm4x8(materials[iMaterialID].cSpecularColor).bgr;
    gfSpecularExponent = materials[iMaterialID].fSpecularExponent;
    gfInstanceSize = fInstanceSize;
}
