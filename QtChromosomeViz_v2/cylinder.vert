#version 330 core

layout(location = 0) in vec3 vVertexPosition;
layout(location = 1) in vec3 vVertexNormal;
layout(location = 2) in vec3 vInstancePosition;
layout(location = 3) in vec4 vInstanceRotation;
layout(location = 4) in uvec2 cInstanceColor;
layout(location = 5) in uvec2 cInstanceSpecularColor;
layout(location = 6) in vec2 fInstanceSpecularExponent;
layout(location = 7) in vec3 fInstanceSize;

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
    vPosition = pos;
    vViewPosition = mv * objectSpacePos;
    vNormal = normalize(mvNormal * -rotate_vector(vInstanceRotation.wxyz, vVertexNormal));
    iInstanceID = 0u;
    iFlags = 0u;
    cColor = mix(colorFromARGB8(cInstanceColor.x | 0xFF000000U),
                 colorFromARGB8(cInstanceColor.y | 0xFF000000U),
                 blendFactor);
    cSpecularColor = mix(colorFromARGB8(cInstanceSpecularColor.x).rgb,
                         colorFromARGB8(cInstanceSpecularColor.y).rgb,
                         blendFactor);
    fSpecularExponent = mix(fInstanceSpecularExponent.x,
                            fInstanceSpecularExponent.y,
                            blendFactor);
}
