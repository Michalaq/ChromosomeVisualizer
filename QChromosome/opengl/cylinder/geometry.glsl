#version 410 core

layout (lines) in;
layout (triangle_strip, max_vertices = 20) out;

layout (std140) uniform shader_data
{
    mat4 pro;
    int pro_flagBits; 
    mat4 mv;
    int mv_flagBits;
    ivec2 uvScreenSize;
};

flat in vec3 gvInstancePosition[2];
flat in int giFlags[2];
flat in vec4 gcColor[2];
flat in vec3 gcSpecularColor[2];
flat in float gfSpecularExponent[2];
flat in float gfInstanceRadius[2];

out vec4 vPosition;
out vec4 vViewPosition;
out vec3 vNormal;
flat out int iFlags;
out vec4 cColor;
out vec3 cSpecularColor;
out float fSpecularExponent;

vec3 rotatedVector(vec4 q, vec3 v) {
    return 2. * dot(q.xyz, v) * q.xyz + (q.w * q.w - dot(q.xyz, q.xyz)) * v + 2. * q.w * cross(q.xyz, v);
}

vec4 rotationTo(vec3 u, vec3 v) {
    vec4 q = vec4(cross(u, v), dot(u, v));
    q.w += length(q);
    return q == vec4(0, 0, 0, 0) ? vec4(1, 0, 0, 0) : normalize(q);
}

const float PI = 3.1415926535897932384626433832795;

void main() {
    iFlags = giFlags[0] & giFlags[1];
    
    if ((iFlags & 0x2) == 0x2)
    {
        vec3 vInstancePosition = (gvInstancePosition[0] + gvInstancePosition[1]) / 2;
        vec4 vInstanceRotation = rotationTo(vec3(0, 0, 1), normalize(gvInstancePosition[0] - gvInstancePosition[1]));
        float fInstanceSize = length(gvInstancePosition[0] - gvInstancePosition[1]) / 2;
        
        vec3 vVertexPosition;
        vec3 vVertexNormal;
        
        mat3 mvNormal = transpose(inverse(mat3(mv)));
        
        for (int i = 0; i < 10; i++)
        {
            float a = 2*PI*i/9;
            
            vNormal = rotatedVector(vInstanceRotation, vec3(cos(a), sin(a), 0));
            vViewPosition = mv * vec4(gvInstancePosition[0] + gfInstanceRadius[0] * vNormal, 1.0);
            vNormal = mvNormal * vNormal;
            gl_Position = vPosition = pro * vViewPosition;
            cColor = gcColor[0];
            cSpecularColor = gcSpecularColor[0];
            fSpecularExponent = gfSpecularExponent[0];
        
            EmitVertex();
            
            vNormal = rotatedVector(vInstanceRotation, vec3(cos(a), sin(a), 0));
            vViewPosition = mv * vec4(gvInstancePosition[1] + gfInstanceRadius[1] * vNormal, 1.0);
            vNormal = mvNormal * vNormal;
            gl_Position = vPosition = pro * vViewPosition;
            cColor = gcColor[1];
            cSpecularColor = gcSpecularColor[1];
            fSpecularExponent = gfSpecularExponent[1];
        
            EmitVertex();
        }
        
        EndPrimitive();
    }
}
