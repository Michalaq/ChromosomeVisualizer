#version 410 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform vec2 uvScreenSize;

out vec2 glPosition;

void main() 
{
    float s = min(uvScreenSize.x, uvScreenSize.y) * 0.8;
    
    glPosition = vec2( 1., 1.);
    gl_Position = vec4(glPosition * s / uvScreenSize, 0, 1);
    EmitVertex();

    glPosition = vec2(-1., 1.);
    gl_Position = vec4(glPosition * s / uvScreenSize, 0, 1);
    EmitVertex();

    glPosition = vec2( 1.,-1.);
    gl_Position = vec4(glPosition * s / uvScreenSize, 0, 1);
    EmitVertex();

    glPosition = vec2(-1.,-1.);
    gl_Position = vec4(glPosition * s / uvScreenSize, 0, 1);
    EmitVertex();

    EndPrimitive(); 
}
