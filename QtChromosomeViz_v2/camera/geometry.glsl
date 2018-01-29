#version 330 core

layout (points) in;
layout (line_strip) out;
layout (max_vertices = 58) out;

uniform mat4 pro;
uniform mat4 mv;

flat in mat4 gModelview[1];
flat in mat4 gProjection[1];
flat in uint giFlags[1];

vec3 tab[] = vec3[](
    vec3(+10.625, -10.625, 0), vec3(+10.625, +10.625, 0), vec3(-10.625, +10.625, 0), vec3(-10.625, -10.625, 0),
    vec3(+3.75, -3.75, 12.5), vec3(+3.75, +3.75, 12.5), vec3(-3.75, +3.75, 12.5), vec3(-3.75, -3.75, 12.5),
    vec3(+12.5, -12.5, 12.5), vec3(+12.5, +12.5, 12.5), vec3(-12.5, +12.5, 12.5), vec3(-12.5, -12.5, 12.5),
    vec3(+12.5, -12.5, 62.5), vec3(+12.5, +12.5, 62.5), vec3(-12.5, +12.5, 62.5), vec3(-12.5, -12.5, 62.5),
    vec3(0, 25, 25), vec3(0, 25, 50)
);

const float PI = 3.1415926535897932384626433832795;

void main() {
    if ((giFlags[0] & 2u) == 2u)
    {
        gl_Position = pro * mv * vec4(tab[0], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[1], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[2], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[3], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[0], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[4], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[5], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[6], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[7], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[4], 1); EmitVertex();
        EndPrimitive();
        gl_Position = pro * mv * vec4(tab[1], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[5], 1); EmitVertex();
        EndPrimitive();
        gl_Position = pro * mv * vec4(tab[2], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[6], 1); EmitVertex();
        EndPrimitive();
        gl_Position = pro * mv * vec4(tab[3], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[7], 1); EmitVertex();
        EndPrimitive();
        gl_Position = pro * mv * vec4(tab[8], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[9], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[10], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[11], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[8], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[12], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[13], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[14], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[15], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[12], 1); EmitVertex();
        EndPrimitive();
        gl_Position = pro * mv * vec4(tab[9], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[13], 1); EmitVertex();
        EndPrimitive();
        gl_Position = pro * mv * vec4(tab[10], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[14], 1); EmitVertex();
        EndPrimitive();
        gl_Position = pro * mv * vec4(tab[11], 1); EmitVertex();
        gl_Position = pro * mv * vec4(tab[15], 1); EmitVertex();
        EndPrimitive();
        for (int i = 0; i < 13; i++) {gl_Position = pro * mv * vec4(tab[16] + 12.5 * vec3(0, cos(PI/6*i), sin(PI/6*i)), 1); EmitVertex();}
        EndPrimitive();
        for (int i = 0; i < 13; i++) {gl_Position = pro * mv * vec4(tab[17] + 12.5 * vec3(0, cos(PI/6*i), sin(PI/6*i)), 1); EmitVertex();}
        EndPrimitive();
    }
}
