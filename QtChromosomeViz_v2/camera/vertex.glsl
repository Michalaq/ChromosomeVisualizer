#version 330 core

layout(location = 0) in mat4 modelView;
layout(location = 4) in mat4 projection;
layout(location = 8) in int iInstanceFlags;

flat out mat4 gModelView;
flat out mat4 gProjection;
flat out int giFlags;

void main() {
    gModelView = modelView;
    gProjection = projection;
    giFlags = iInstanceFlags;
}
