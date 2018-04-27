#version 330 core

layout(location = 0) in vec3 vInstancePosition;
layout(location = 1) in int iInstanceFlags;
layout(location = 2) in float fInstanceSize;
layout(location = 3) in ivec4 vLabelRect;

flat out vec3 gvInstancePosition;
flat out int giFlags;
flat out float gfInstanceSize;
flat out ivec4 gvLabelRect;

void main() {
    gvInstancePosition = vInstancePosition;
    giFlags = iInstanceFlags;
    gfInstanceSize = fInstanceSize;
    gvLabelRect = vLabelRect;
}
