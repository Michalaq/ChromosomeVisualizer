#version 330 core

layout(location = 0) in vec3 vVertexPosition;
layout(location = 1) in vec3 vVertexNormal;
layout(location = 2) in vec3 vInstancePosition;
layout(location = 3) in uvec2 iInstaceAttr; // (iInstanceFlags,iAtomID)
layout(location = 4) in vec4 cInstanceColor;
layout(location = 5) in vec4 cInstanceSpecularColor;
layout(location = 6) in vec2 fInstanceAttr; // (fInstanceSpecularExponent,fInstanceSize)

uniform mat4 pro;
uniform mat4 mv;
out vec3 gvViewPosition;
flat out vec3 gvInstancePosition;
flat out uint giInstanceID;
flat out uint giFlags;
flat out vec4 gcColor;
flat out vec3 gcSpecularColor;
flat out float gfSpecularExponent;
flat out float gfInstanceSize;

void main() {
    vec4 objectSpacePos = mv * vec4(vVertexPosition * fInstanceAttr.y + vInstancePosition.xyz, 1);
    gvViewPosition = objectSpacePos.xyz / objectSpacePos.w;
    gl_Position = pro * objectSpacePos;
    vec4 objectSpaceInstancePos = mv * vec4(vInstancePosition, 1);
    gvInstancePosition = objectSpaceInstancePos.xyz / objectSpaceInstancePos.w;
    giInstanceID = iInstaceAttr.y;
    giFlags = iInstaceAttr.x;
    gcColor = cInstanceColor.bgra;
    gcSpecularColor = cInstanceSpecularColor.bgr;
    gfSpecularExponent = fInstanceAttr.x;
    gfInstanceSize = fInstanceAttr.y;
}
