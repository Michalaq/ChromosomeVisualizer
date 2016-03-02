#version 330 core

in vec2 vTexCoord;
uniform sampler2D sLabel;
out vec4 cColor;

void main() {
    cColor = texture2D(sLabel, vTexCoord);
}
