#version 330 core

layout(location = 0) in vec3 vInstancePosition;
layout(location = 1) in uvec4 iInstaceAttr; // (iInstanceFlags,iAtomID,cInstanceColor,cInstanceSpecularColor)
layout(location = 2) in vec2 fInstanceAttr; // (fInstanceSpecularExponent,fInstanceSize)

flat out vec3 gvInstancePosition;
flat out uint giFlags;
flat out uint giInstanceID;
flat out vec4 gcColor;
flat out vec3 gcSpecularColor;
flat out float gfSpecularExponent;
flat out float gfInstanceSize;

vec4 colorFromARGB8(uint color) {
    vec4 ret;
    ret.a = float((color >> 24u) & 0xFFu) / 255.f;
    ret.r = float((color >> 16u) & 0xFFu) / 255.f;
    ret.g = float((color >>  8u) & 0xFFu) / 255.f;
    ret.b = float((color >>  0u) & 0xFFu) / 255.f;
    return ret;
}

void main() {
    gvInstancePosition = vInstancePosition;
    giFlags = iInstaceAttr.x;
    giInstanceID = iInstaceAttr.y;
    gcColor = colorFromARGB8(iInstaceAttr.z);
    gcSpecularColor = colorFromARGB8(iInstaceAttr.w).rgb;
    gfSpecularExponent = fInstanceAttr.x;
    gfInstanceSize = fInstanceAttr.y;
}
