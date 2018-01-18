#version 330 core

layout(location = 0) in vec3 vVertexPosition;
layout(location = 1) in vec3 vVertexNormal;
layout(location = 2) in vec3 vInstancePosition;
layout(location = 3) in uvec4 iInstaceAttr; // (iInstanceFlags,iAtomID,cInstanceColor,cInstanceSpecularColor)
layout(location = 4) in vec2 fInstanceAttr; // (fInstanceSpecularExponent,fInstanceSize)

uniform mat4 mvp;
uniform mat4 mv;
out vec4 gvViewPosition;
flat out vec3 gvInstancePosition;
flat out uint giInstanceID;
flat out uint giFlags;
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
    vec4 objectSpacePos = vec4(vVertexPosition * fInstanceAttr.y + vInstancePosition.xyz, 1);
    gl_Position = mvp * objectSpacePos;
    gvViewPosition = mv * objectSpacePos;
    gvInstancePosition = vInstancePosition;
    giInstanceID = iInstaceAttr.y;
    giFlags = iInstaceAttr.x;
    gcColor = colorFromARGB8(iInstaceAttr.z);
    gcSpecularColor = colorFromARGB8(iInstaceAttr.w).rgb;
    gfSpecularExponent = fInstanceAttr.x;
    gfInstanceSize = fInstanceAttr.y;
}
