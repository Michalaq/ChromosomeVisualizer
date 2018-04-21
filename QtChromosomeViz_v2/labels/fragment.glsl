#version 330 core

uniform sampler2D SampleTexture;

in vec2 vTextureCoord;

out vec4 fragColor;

void main() {
    vec2 vTextureCoord1 = (vTextureCoord + vec2(1,1))/2;
    fragColor = texture(SampleTexture,vTextureCoord1);
    return;
}
