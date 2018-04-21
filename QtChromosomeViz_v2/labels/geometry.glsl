#version 330 core

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 6) out;

uniform mat4 pro;
uniform mat4 mv;

flat in vec3 gvInstancePosition[1];
flat in int giFlags[1];
flat in float gfInstanceSize[1];

out vec2 vTextureCoord;

// this can be passed from CPU (as uniform?) 
vec2 tetrahedron[4] = vec2[](
    vec2(+1, +1),
    vec2(+1, -1),
    vec2(-1, +1),
    vec2(-1, -1)
);

void main() {
    if ((giFlags[0] & 0x2) == 0x2)
    {
        vec4 objectSpacePos = mv * vec4(gvInstancePosition[0], 1.0);
        
        vec3 vInstancePosition = objectSpacePos.xyz / objectSpacePos.w;
        
        vec4 tmp = pro * vec4(vInstancePosition, 1.0);
        vec3 foo = tmp.xyz / tmp.w;
        
        for(int i = 0; i < 6; i++)
        {
            vTextureCoord = tetrahedron[i & 3];
            gl_Position = vec4(foo + vec3(vTextureCoord, 0) * .1, 1.0);
            EmitVertex();
        }
        
        EndPrimitive();
    }
}
