#version 330 core

uniform vec2 uvScreenSize;
in vec4 vPosition;
in vec3 vNormal;
flat in uint iFlags;
in vec4 vColor;
out vec4 cColor;

void main() {
    vec2 vScreenPos = 0.5f * (vPosition.xy * uvScreenSize) / vPosition.w;
    float stripePhase = 0.5f * (vScreenPos.x + vScreenPos.y);
    float whitening = clamp(0.5f * (3.f * sin(stripePhase)), 0.f, 0.666f);
    vec4 baseColor = vColor;
    float lightness = (0.5 + 0.5 * 0.7 * (vNormal.x + vNormal.z));
    vec4 cDiffuse = vec4(baseColor.xyz * lightness, baseColor.a);
    float isSelected = ((iFlags & 4u) == 4u) ? 1.f : 0.f;
    cColor = mix(cDiffuse, vec4(1.f, 1.f, 1.f, 1.f), isSelected * whitening);
}
