#version 410 core

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 6) out;

layout (std140) uniform shader_data
{
    mat4 pro;
    int pro_flagBits; 
    mat4 mv;
    int mv_flagBits;
    ivec2 uvScreenSize;
};

flat in vec3 gvInstancePosition[1];
flat in int giFlags[1];
flat in float gfInstanceSize[1];
flat in ivec4 gvLabelRect[1];

out vec2 vTextureCoord;

// this can be passed from CPU (as uniform?) 
vec2 tetrahedron[4] = vec2[](
    vec2(+1, +1),
    vec2(+1, -1),
    vec2(-1, +1),
    vec2(-1, -1)
);

void main() {
    if ((giFlags[0] & 0x2) == 0x2 && gvLabelRect[0].z >= 0 && gvLabelRect[0].w >= 0)
    {
        vec4 objectSpacePos = mv * vec4(gvInstancePosition[0], 1.0);
        
        vec3 vInstancePosition = objectSpacePos.xyz / objectSpacePos.w;
        vec3 bar = normalize(vInstancePosition);
        vInstancePosition += bar * gfInstanceSize[0] / bar.z;
        
        vec4 tmp = pro * vec4(vInstancePosition, 1.0);
        vec3 foo = tmp.xyz / tmp.w;
        
        vec2 rect = vec2(gvLabelRect[0].z - gvLabelRect[0].x + 1, gvLabelRect[0].w - gvLabelRect[0].y + 1);
        
        for(int i = 0; i < 6; i++)
        {
            vTextureCoord = (tetrahedron[i & 3] + vec2(1, 1)) / 2 * rect + gvLabelRect[0].xy;
            gl_Position = vec4(foo + vec3(tetrahedron[i & 3] * rect / uvScreenSize, 0), 1.0);
            EmitVertex();
        }
        
        EndPrimitive();
    }
}
