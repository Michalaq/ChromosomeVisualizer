#version 330 core

layout(location = 0) in mat4 modelview;
layout(location = 1) in mat4 projection;

flat out vec3 gvInstancePosition;
flat out uint giFlags;
flat out uint giInstanceID;
flat out vec4 gcColor;
flat out vec3 gcSpecularColor;
flat out float gfSpecularExponent;
flat out float gfInstanceSize;

void main() {
    gvInstancePosition = vec3(0, 0, 0);
    giFlags = 2u;
    giInstanceID = 0u;
    gcColor = vec4(1,0,0,1);
    gcSpecularColor = vec3(1,1,1);
    gfSpecularExponent = 0.4;
    gfInstanceSize = 3;
}
