#version 330 core

layout(location = 0) in mat4 modelView;

flat out mat4 gModelView;
flat out mat4 gProjection;
flat out uint giFlags;

void main() {
    gModelView = modelView;
    //gProjection = projection;
    giFlags = 2u;
}
