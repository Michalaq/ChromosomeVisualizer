#version 410 core

uniform sampler2D SampleTexture;

in vec2 vTextureCoord;

out vec4 fragColor;

void main() {
    fragColor = texelFetch(SampleTexture, ivec2(vTextureCoord), 0);
}
