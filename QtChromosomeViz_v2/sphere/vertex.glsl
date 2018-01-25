#version 330 core

layout(location = 0) in vec3 vInstancePosition;
layout(location = 1) in uvec2 iInstaceAttr; // (iInstanceFlags,iAtomID)
layout(location = 2) in vec4 cInstanceColor;
layout(location = 3) in vec4 cInstanceSpecularColor;
layout(location = 4) in vec2 fInstanceAttr; // (fInstanceSpecularExponent,fInstanceSize)

flat out vec3 gvInstancePosition;
flat out uint giInstanceID;
flat out uint giFlags;
flat out vec4 gcColor;
flat out vec3 gcSpecularColor;
flat out float gfSpecularExponent;
flat out float gfInstanceSize;

void main() {
    gvInstancePosition = vInstancePosition;
    giInstanceID = iInstaceAttr.y;
    giFlags = iInstaceAttr.x;
    gcColor = cInstanceColor.bgra;
    gcSpecularColor = cInstanceSpecularColor.bgr;
    gfSpecularExponent = fInstanceAttr.x;
    gfInstanceSize = fInstanceAttr.y;
}
