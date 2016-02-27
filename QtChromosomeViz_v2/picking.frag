#version 330 core

flat in uint iInstanceID;
out vec4 cColor;

void main() {
    cColor.a = float((iInstanceID >> 24u) & 0xFFu) / 255.f;
    cColor.r = float((iInstanceID >> 16u) & 0xFFu) / 255.f;
    cColor.g = float((iInstanceID >>  8u) & 0xFFu) / 255.f;
    cColor.b = float((iInstanceID >>  0u) & 0xFFu) / 255.f;
}