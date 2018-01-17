#version 330 core

layout(location = 0) in vec3 vVertexPosition;
layout(location = 1) in vec3 vVertexNormal;
layout(location = 2) in vec3 vInstancePosition1;
layout(location = 3) in uint iInstanceFlags1;
layout(location = 4) in uint cInstanceColor1;
layout(location = 5) in uint cInstanceSpecularColor1;
layout(location = 6) in float fInstanceSpecularExponent1;
layout(location = 7) in float fInstanceSize1;
layout(location = 8) in vec3 vInstancePosition2;
layout(location = 9) in uint iInstanceFlags2;
layout(location = 10) in uint cInstanceColor2;
layout(location = 11) in uint cInstanceSpecularColor2;
layout(location = 12) in float fInstanceSpecularExponent2;
layout(location = 13) in float fInstanceSize2;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat3 mvNormal;
out vec4 gvPosition;
out vec4 gvViewPosition;
out vec3 gvNormal;
flat out uint giInstanceID;
flat out uint giFlags;
out vec4 gcColor;
out vec3 gcSpecularColor;
out float gfSpecularExponent;
flat out uint giVisible;

vec4 colorFromARGB8(uint color) {
    vec4 ret;
    ret.a = float((color >> 24u) & 0xFFu) / 255.f;
    ret.r = float((color >> 16u) & 0xFFu) / 255.f;
    ret.g = float((color >>  8u) & 0xFFu) / 255.f;
    ret.b = float((color >>  0u) & 0xFFu) / 255.f;
    return ret;
}

vec3 rotatedVector(vec4 q, vec3 v) {
    return 2. * dot(q.xyz, v) * q.xyz + (q.w * q.w - dot(q.xyz, q.xyz)) * v + 2. * q.w * cross(q.xyz, v);
}

vec4 rotationTo(vec3 u, vec3 v) {
    vec4 q = vec4(cross(u, v), dot(u, v));
    q.w += length(q);
    return normalize(q);
}

void main() {
    vec3 vInstancePosition = (vInstancePosition1 + vInstancePosition2) / 2;
    vec4 vInstanceRotation = rotationTo(vec3(0, 0, 1), normalize(vInstancePosition1 - vInstancePosition2));
    float fInstanceSize = length(vInstancePosition1 - vInstancePosition2) / 2;
    
    float blendFactor = 0.5 + 0.5 * vVertexPosition.z;
    float size = mix(fInstanceSize1, fInstanceSize2, blendFactor) * 0.5;
    vec3 pos3 = vVertexPosition * -vec3(size, size, fInstanceSize);
    pos3 = rotatedVector(vInstanceRotation, pos3);
    vec4 objectSpacePos = vec4(pos3 + vInstancePosition.xyz, 1);
    vec4 pos = mvp * objectSpacePos;
    gl_Position = pos;
    gvPosition = pos;
    gvViewPosition = mv * objectSpacePos;
    gvNormal = normalize(mvNormal * -rotatedVector(vInstanceRotation, vVertexNormal));
    giInstanceID = 0u;
    giFlags = 0u;
    gcColor = mix(colorFromARGB8(cInstanceColor1),
                  colorFromARGB8(cInstanceColor2),
                  blendFactor);
    gcSpecularColor = mix(colorFromARGB8(cInstanceSpecularColor1).rgb,
                          colorFromARGB8(cInstanceSpecularColor2).rgb,
                          blendFactor);
    gfSpecularExponent = mix(fInstanceSpecularExponent1,
                             fInstanceSpecularExponent2,
                             blendFactor);
    giVisible = iInstanceFlags1 & iInstanceFlags2;
}
