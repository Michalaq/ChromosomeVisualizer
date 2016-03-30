#version 330 core

layout(location = 0) in vec3 vVertexPosition;
layout(location = 1) in vec3 vVertexNormal;
layout(location = 2) in vec3 vInstancePosition;
layout(location = 3) in uint iInstanceFlags;
layout(location = 4) in uint iAtomID;
layout(location = 5) in uint cInstanceColor;
layout(location = 6) in uint cInstanceSpecularColor;
layout(location = 7) in float fInstanceSpecularExponent;
layout(location = 8) in float fInstanceSize;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat3 mvNormal;
out vec4 vPosition;
out vec4 vViewPosition;
out vec3 vNormal;
flat out uint iInstanceID;
flat out uint iFlags;
out vec4 cColor;
out vec3 cSpecularColor;
out float fSpecularExponent;

vec4 colorFromARGB8(uint color) {
    vec4 ret;
    ret.a = float((color >> 24u) & 0xFFu) / 255.f;
    ret.r = float((color >> 16u) & 0xFFu) / 255.f;
    ret.g = float((color >>  8u) & 0xFFu) / 255.f;
    ret.b = float((color >>  0u) & 0xFFu) / 255.f;
    return ret;
}

void main() {
    vec4 objectSpacePos = vec4(vVertexPosition * fInstanceSize + vInstancePosition.xyz, 1);
    vec4 pos = mvp * objectSpacePos;
    gl_Position = pos;
    vPosition = pos;
    vViewPosition = mv * objectSpacePos;
    vNormal = normalize(mvNormal * vVertexNormal);
    iInstanceID = iAtomID;
    iFlags = iInstanceFlags;
    cColor = colorFromARGB8(cInstanceColor);
    cSpecularColor = colorFromARGB8(cInstanceSpecularColor).rgb;
    fSpecularExponent = fInstanceSpecularExponent;
}
