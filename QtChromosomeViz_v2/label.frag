#version 330 core

in vec2 vTexCoord;
uniform sampler2D sLabel;
uniform vec4 ucTextColor;
uniform vec4 ucBackgroundColor;
out vec4 cColor;

void main() {
    cColor = mix(ucBackgroundColor, ucTextColor, texture2D(sLabel, vTexCoord).a);
}
