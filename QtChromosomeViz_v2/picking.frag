#version 330 core

layout (std140) uniform shader_data
{
    mat4 pro;
    int pro_flagBits; 
    mat4 mv;
    int mv_flagBits;
    ivec2 uvScreenSize;
    float ufFogDensity;
    float ufFogContribution;
    uint ucFogColor;
};

in vec3 vViewPosition;
flat in vec3 vInstancePosition;
flat in int iInstanceID;
flat in float fInstanceSize;

out vec4 cColor;

void main() {
    float p = dot(vViewPosition, vViewPosition);
    float q = dot(vViewPosition, vInstancePosition);
    float r = dot(vInstancePosition, vInstancePosition);

    float d = q * q - p * (r - fInstanceSize * fInstanceSize);
    
    if (d < 0.0)
        discard;
    
    float s = sign(p - q);
    float t = (q - s * sqrt(d)) / p;

    vec3 vNormal = s * (t * vViewPosition - vInstancePosition) / fInstanceSize;
    vec4 vFragCoord = pro * vec4(t * vViewPosition, 1.0);
    
    gl_FragDepth = 0.5 * vFragCoord.z / vFragCoord.w + 0.5;

    cColor.a = float((iInstanceID >> 24) & 0xFF) / 255.f;
    cColor.r = float((iInstanceID >> 16) & 0xFF) / 255.f;
    cColor.g = float((iInstanceID >>  8) & 0xFF) / 255.f;
    cColor.b = float((iInstanceID >>  0) & 0xFF) / 255.f;
}
