#version 330 core

layout(location = 0) in vec3 vInstancePosition;
layout(location = 1) in uint iInstanceFlags;
layout(location = 2) in vec4 cInstanceColor;
layout(location = 3) in vec4 cInstanceSpecularColor;
layout(location = 4) in float fInstanceSpecularExponent;
layout(location = 5) in float fInstanceSize;

flat out vec3 gvInstancePosition;
flat out uint giFlags;
flat out int giInstanceID;
flat out vec4 gcColor;
flat out vec3 gcSpecularColor;
flat out float gfSpecularExponent;
flat out float gfInstanceSize;

void main() {
    gvInstancePosition = vInstancePosition;
    giFlags = iInstanceFlags;
    giInstanceID = gl_VertexID;
    gcColor = cInstanceColor.bgra;
    gcSpecularColor = cInstanceSpecularColor.bgr;
    gfSpecularExponent = fInstanceSpecularExponent;
    gfInstanceSize = fInstanceSize;
}
