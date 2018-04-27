#version 330 core

layout (points) in;
layout (line_strip) out;
layout (max_vertices = 58) out;

uniform mat4 pro;
uniform mat4 mv;

flat in mat4 gModelView[1];
flat in mat4 gProjection[1];
flat in int giFlags[1];

vec3 tab[] = vec3[](
    vec3(+10.625, -10.625, 0), vec3(+10.625, +10.625, 0), vec3(-10.625, +10.625, 0), vec3(-10.625, -10.625, 0),
    vec3(+3.75, -3.75, 12.5), vec3(+3.75, +3.75, 12.5), vec3(-3.75, +3.75, 12.5), vec3(-3.75, -3.75, 12.5),
    vec3(+12.5, -12.5, 12.5), vec3(+12.5, +12.5, 12.5), vec3(-12.5, +12.5, 12.5), vec3(-12.5, -12.5, 12.5),
    vec3(+12.5, -12.5, 62.5), vec3(+12.5, +12.5, 62.5), vec3(-12.5, +12.5, 62.5), vec3(-12.5, -12.5, 62.5),
    vec3(0, 25, 25), vec3(0, 25, 50)
);

const float PI = 3.1415926535897932384626433832795;

void main() {
    if ((giFlags[0] & 0x2) == 0x2)
    {
        mat4 tmp = mat4(0.1); tmp[3][3] = 1;
        mat4 mat = pro * mv * inverse(gModelView[0]) * tmp;
        
        gl_Position = mat * vec4(tab[0], 1); EmitVertex();
        gl_Position = mat * vec4(tab[1], 1); EmitVertex();
        gl_Position = mat * vec4(tab[2], 1); EmitVertex();
        gl_Position = mat * vec4(tab[3], 1); EmitVertex();
        gl_Position = mat * vec4(tab[0], 1); EmitVertex();
        gl_Position = mat * vec4(tab[4], 1); EmitVertex();
        gl_Position = mat * vec4(tab[5], 1); EmitVertex();
        gl_Position = mat * vec4(tab[6], 1); EmitVertex();
        gl_Position = mat * vec4(tab[7], 1); EmitVertex();
        gl_Position = mat * vec4(tab[4], 1); EmitVertex();
        EndPrimitive();
        gl_Position = mat * vec4(tab[1], 1); EmitVertex();
        gl_Position = mat * vec4(tab[5], 1); EmitVertex();
        EndPrimitive();
        gl_Position = mat * vec4(tab[2], 1); EmitVertex();
        gl_Position = mat * vec4(tab[6], 1); EmitVertex();
        EndPrimitive();
        gl_Position = mat * vec4(tab[3], 1); EmitVertex();
        gl_Position = mat * vec4(tab[7], 1); EmitVertex();
        EndPrimitive();
        gl_Position = mat * vec4(tab[8], 1); EmitVertex();
        gl_Position = mat * vec4(tab[9], 1); EmitVertex();
        gl_Position = mat * vec4(tab[10], 1); EmitVertex();
        gl_Position = mat * vec4(tab[11], 1); EmitVertex();
        gl_Position = mat * vec4(tab[8], 1); EmitVertex();
        gl_Position = mat * vec4(tab[12], 1); EmitVertex();
        gl_Position = mat * vec4(tab[13], 1); EmitVertex();
        gl_Position = mat * vec4(tab[14], 1); EmitVertex();
        gl_Position = mat * vec4(tab[15], 1); EmitVertex();
        gl_Position = mat * vec4(tab[12], 1); EmitVertex();
        EndPrimitive();
        gl_Position = mat * vec4(tab[9], 1); EmitVertex();
        gl_Position = mat * vec4(tab[13], 1); EmitVertex();
        EndPrimitive();
        gl_Position = mat * vec4(tab[10], 1); EmitVertex();
        gl_Position = mat * vec4(tab[14], 1); EmitVertex();
        EndPrimitive();
        gl_Position = mat * vec4(tab[11], 1); EmitVertex();
        gl_Position = mat * vec4(tab[15], 1); EmitVertex();
        EndPrimitive();
        for (int i = 0; i < 13; i++) {gl_Position = mat * vec4(tab[16] + 12.5 * vec3(0, cos(PI/6*i), sin(PI/6*i)), 1); EmitVertex();}
        EndPrimitive();
        for (int i = 0; i < 13; i++) {gl_Position = mat * vec4(tab[17] + 12.5 * vec3(0, cos(PI/6*i), sin(PI/6*i)), 1); EmitVertex();}
        EndPrimitive();
    }
}
