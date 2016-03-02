#version 330 core

uniform vec2 uvCenter;
uniform vec2 uvExtents;

out vec2 vTexCoord;

void main() {
    vec2 mask = vec2(gl_VertexID & 1, (gl_VertexID & 2) / 2);
    gl_Position = vec4(uvCenter + (2 * mask - 1) * uvExtents, 0, 1);
    vTexCoord = mask;
}
