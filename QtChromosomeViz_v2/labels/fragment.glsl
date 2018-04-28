#version 420 core

uniform sampler2D SampleTexture;

in vec2 vTextureCoord;

out vec4 fragColor;

void main() {
    fragColor = texture(SampleTexture, vTextureCoord);
    if (fragColor.a < 0.01) discard;
}
