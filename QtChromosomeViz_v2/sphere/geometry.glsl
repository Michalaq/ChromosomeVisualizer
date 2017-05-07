#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

in vec4 gvViewPosition[3];
flat in vec3 gvInstancePosition[3];
flat in uint giInstanceID[3];
flat in uint giFlags[3];
flat in vec4 gcColor[3];
flat in vec3 gcSpecularColor[3];
flat in float gfSpecularExponent[3];
flat in float gfInstanceSize[3];

out vec4 vPosition;
out vec4 vViewPosition;
flat out vec3 vInstancePosition;
flat out uint iInstanceID;
flat out uint iFlags;
flat out vec4 cColor;
flat out vec3 cSpecularColor;
flat out float fSpecularExponent;
flat out float fInstanceSize;

void main() {
    if ((giFlags[0] & 8u) == 8u)
    {
        vInstancePosition = gvInstancePosition[0];
        iInstanceID = giInstanceID[0];
        iFlags = giFlags[0];
        cColor = gcColor[0];
        cSpecularColor = gcSpecularColor[0];
        fSpecularExponent = gfSpecularExponent[0];
        fInstanceSize = gfInstanceSize[0];
        
        for(int i = 0; i < 3; i++)
        {
            gl_Position = vPosition = gl_in[i].gl_Position;
            
            vViewPosition = gvViewPosition[i];
            vInstancePosition = gvInstancePosition[i];
            
            EmitVertex();
        }
        
        EndPrimitive();
    }
}
