#version 330 core

layout(location = 0) in mat4 modelview;
layout(location = 1) in mat4 projection;

flat out mat4 gModelview;
flat out mat4 gProjection;
flat out uint giFlags;

void main() {
    gModelview = modelview;
    gProjection = projection;
    giFlags = 2u;
}
