#version 330 core

uniform mat4 pro;

in vec3 vViewPosition;
flat in vec3 vInstancePosition;
flat in uint iInstanceID;
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

    cColor.a = float((iInstanceID >> 24u) & 0xFFu) / 255.f;
    cColor.r = float((iInstanceID >> 16u) & 0xFFu) / 255.f;
    cColor.g = float((iInstanceID >>  8u) & 0xFFu) / 255.f;
    cColor.b = float((iInstanceID >>  0u) & 0xFFu) / 255.f;
}
