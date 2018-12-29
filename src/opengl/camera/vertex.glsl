#version 410 core

layout(location = 0) in mat4 projection;
layout(location = 4) in mat4 modelView;
layout(location = 8) in int iInstanceFlags;

flat out mat4 gProjection;
flat out mat4 gModelView;
flat out int giFlags;

void main() {
    gProjection = projection;
    gModelView = modelView;
    giFlags = iInstanceFlags;
}
