#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

in vec4 gvViewPosition[3];
in vec3 gvNormal[3];
flat in uint giInstanceID[3];
flat in uint giFlags[3];
in vec4 gcColor[3];
in vec3 gcSpecularColor[3];
in float gfSpecularExponent[3];

out vec4 vPosition;
out vec4 vViewPosition;
out vec3 vNormal;
flat out uint iInstanceID;
flat out uint iFlags;
out vec4 cColor;
out vec3 cSpecularColor;
out float fSpecularExponent;

void main() {
    if ((giFlags[0] & 8u) == 8u)
    {
        for(int i = 0; i < 3; i++)
        {
            gl_Position = vPosition = gl_in[i].gl_Position;
            vViewPosition = gvViewPosition[i];
            vNormal = gvNormal[i];
            iInstanceID = giInstanceID[i];
            iFlags = giFlags[i];
            cColor = gcColor[i];
            cSpecularColor = gcSpecularColor[i];
            fSpecularExponent = gfSpecularExponent[i];
            EmitVertex();
        }
        EndPrimitive();
    }
}
