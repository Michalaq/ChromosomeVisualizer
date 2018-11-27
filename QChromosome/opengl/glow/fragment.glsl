#version 410 core

uniform sampler2D SampleTexture;

out vec4 fragColor;

const float c[9] = float[](41, 26, 16, 0, 7, 4, 0, 0, 1);

void main() {
    if (texelFetch(SampleTexture, ivec2(gl_FragCoord.xy), 0).x != 0)
        discard;
    
    float a = 0;
    float b = 0;
    
    for (int dx = -2; dx < 3; dx++)
        for (int dy = -2; dy < 3; dy++)
        {
            float d = texelFetch(SampleTexture, ivec2(gl_FragCoord.xy) + ivec2(dx, dy), 0).x;
            
            if (d != 0)
            {
                b += c[dx * dx + dy * dy];
                if (a < d) a = d;
            }
            
        }
    
    if (a == 0)
        discard;
    
    gl_FragDepth = a;
    
    fragColor = vec4(0.98, 0.74, 0.02, b / 26);
}
