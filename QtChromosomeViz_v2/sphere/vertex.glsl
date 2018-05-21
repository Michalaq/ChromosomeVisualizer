#version 410 core

layout(location = 0) in vec3 vInstancePosition;
layout(location = 1) in int iInstanceFlags;
layout(location = 2) in float fInstanceSize;
layout(location = 3) in int iMaterialID;

flat out vec3 gvInstancePosition;
flat out int giFlags;
flat out int giInstanceID;
flat out float gfInstanceSize;
flat out int giMaterialID;

void main() {
    gvInstancePosition = vInstancePosition;
    giFlags = iInstanceFlags;
    giInstanceID = gl_VertexID;
    gfInstanceSize = fInstanceSize;
    giMaterialID = iMaterialID;
}
