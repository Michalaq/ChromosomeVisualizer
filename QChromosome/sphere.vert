#version 330 core

layout(location = 0) in vec3 vVertexPosition;
layout(location = 1) in vec3 vVertexNormal;
layout(location = 2) in vec3 vInstancePosition;
layout(location = 3) in uint iInstanceFlags;
layout(location = 4) in uint iAtomID;
layout(location = 5) in uint cInstanceColor;
layout(location = 6) in float fInstanceSize;

uniform mat4 mvp;
uniform mat3 mvNormal;
out vec4 vPosition;
out vec3 vNormal;
flat out uint iInstanceID;
flat out uint iFlags;
out vec4 vColor;

vec4 colorFromARGB8(uint color) {
    vec4 ret;
    ret.a = float((color >> 24u) & 0xFFu) / 255.f;
    ret.r = float((color >> 16u) & 0xFFu) / 255.f;
    ret.g = float((color >>  8u) & 0xFFu) / 255.f;
    ret.b = float((color >>  0u) & 0xFFu) / 255.f;
    return ret;
}

void main() {
    vec4 pos = mvp * vec4(vVertexPosition * fInstanceSize + vInstancePosition.xyz, 1);
    gl_Position = pos;
    vPosition = pos;
    vNormal = normalize(mvNormal * vVertexNormal);
    iInstanceID = iAtomID;
    iFlags = iInstanceFlags;
    vColor = colorFromARGB8(cInstanceColor);
}
