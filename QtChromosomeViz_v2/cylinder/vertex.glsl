#version 330 core

layout(location = 0) in vec3 vVertexPosition;
layout(location = 1) in vec3 vVertexNormal;
layout(location = 2) in vec3 vInstancePosition;
layout(location = 3) in vec4 vInstanceRotation;
layout(location = 4) in uvec2 cInstanceColor;
layout(location = 5) in uvec2 cInstanceSpecularColor;
layout(location = 6) in vec2 fInstanceSpecularExponent;
layout(location = 7) in vec3 fInstanceSize;
layout(location = 8) in uint iInstanceVisible;

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

vec3 rotate_vector(vec4 quat, vec3 vec) {
    return vec + 2.0 * cross(cross(vec, quat.xyz) + quat.w * vec, quat.xyz);
}

void main() {
    float blendFactor = 0.5 + 0.5 * vVertexPosition.z;
    float size = mix(fInstanceSize.x, fInstanceSize.y, blendFactor) * 0.5;
    vec3 pos3 = vVertexPosition * -vec3(size, size, fInstanceSize.z);
    pos3 = rotate_vector(vInstanceRotation.wxyz, pos3);
    vec4 objectSpacePos = vec4(pos3 + vInstancePosition.xyz, 1);
    vec4 pos = mvp * objectSpacePos;
    gl_Position = pos;
    gvPosition = pos;
    gvViewPosition = mv * objectSpacePos;
    gvNormal = normalize(mvNormal * -rotate_vector(vInstanceRotation.wxyz, vVertexNormal));
    giInstanceID = 0u;
    giFlags = 0u;
    gcColor = mix(colorFromARGB8(cInstanceColor.x),
                  colorFromARGB8(cInstanceColor.y),
                  blendFactor);
    gcSpecularColor = mix(colorFromARGB8(cInstanceSpecularColor.x).rgb,
                          colorFromARGB8(cInstanceSpecularColor.y).rgb,
                          blendFactor);
    gfSpecularExponent = mix(fInstanceSpecularExponent.x,
                             fInstanceSpecularExponent.y,
                             blendFactor);
    giVisible = iInstanceVisible;
}
