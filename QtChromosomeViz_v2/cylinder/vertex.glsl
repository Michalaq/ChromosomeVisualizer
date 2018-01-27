#version 330 core

layout(location = 0) in vec3 vInstancePosition;
layout(location = 1) in uint iInstanceFlags;
layout(location = 2) in uint iAtomID;
layout(location = 3) in vec4 cInstanceColor;
layout(location = 4) in vec4 cInstanceSpecularColor;
layout(location = 5) in float fInstanceSpecularExponent;
layout(location = 6) in float fInstanceSize;

flat out vec3 gvInstancePosition;
flat out uint giFlags;
flat out uint giInstanceID;
flat out vec4 gcColor;
flat out vec3 gcSpecularColor;
flat out float gfSpecularExponent;
flat out float gfInstanceSize;

void main() {
    gvInstancePosition = vInstancePosition;
    giFlags = iInstanceFlags;
    giInstanceID = iAtomID;
    gcColor = cInstanceColor.bgra;
    gcSpecularColor = cInstanceSpecularColor.bgr;
    gfSpecularExponent = fInstanceSpecularExponent;
    gfInstanceSize = fInstanceSize;
}
