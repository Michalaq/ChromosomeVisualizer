#version 420 core

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 6) out;

layout (std140) uniform shader_data
{
    mat4 pro;
    int pro_flagBits; 
    mat4 mv;
    int mv_flagBits;
    mat3 mvNormal;
    ivec2 uvScreenSize;
    float ufFogDensity;
    float ufFogContribution;
    uint ucFogColor;
};

flat in vec3 gvInstancePosition[1];
flat in int giFlags[1];
flat in int giInstanceID[1];
flat in vec4 gcColor[1];
flat in vec3 gcSpecularColor[1];
flat in float gfSpecularExponent[1];
flat in float gfInstanceSize[1];

out vec4 vPosition;
out vec3 vViewPosition;

flat out vec3 vInstancePosition;
flat out int iFlags;
flat out int iInstanceID;
flat out vec4 cColor;
flat out vec3 cSpecularColor;
flat out float fSpecularExponent;
flat out float fInstanceSize;

// this can be passed from CPU (as uniform?) 
vec3 tetrahedron[4] = vec3[](
    vec3(+1, +1, +1),
    vec3(-1, -1, +1),
    vec3(-1, +1, -1),
    vec3(+1, -1, -1)
);

void main() {
    if ((giFlags[0] & 0x2) == 0x2)
    {
        vec4 objectSpacePos = mv * vec4(gvInstancePosition[0], 1.0);
        
        vInstancePosition = objectSpacePos.xyz / objectSpacePos.w;
        iFlags = giFlags[0];
        iInstanceID = giInstanceID[0];
        cColor = gcColor[0];
        cSpecularColor = gcSpecularColor[0];
        fSpecularExponent = gfSpecularExponent[0];
        fInstanceSize = gfInstanceSize[0];
        
        for(int i = 0; i < 6; i++)
        {
            vViewPosition = vInstancePosition + tetrahedron[i & 3] * fInstanceSize * 3.5;
            gl_Position = vPosition = pro * vec4(vViewPosition, 1.0);
            
            EmitVertex();
        }
        
        EndPrimitive();
    }
}
