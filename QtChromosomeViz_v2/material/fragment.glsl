#version 330 core

uniform vec4 cColor;
uniform vec3 cSpecularColor;
uniform float fSpecularExponent;

in vec2 position;

const float r = 5. / 6.;
const float r2 = r * r;

void main() {
    float stripePhase = floor((position.x - position.y) * 2.5);
    vec3 background = (int(stripePhase) & 1) == 0 ? vec3(.4, .4, .4) : vec3(.6, .6, .6);
    
    if (dot(position, position) < r2)
    {
        const vec3 cvLightDirection = normalize(vec3(-1., 1., -2.));
        vec4 baseColor = cColor;
        
        vec3 ocColor[2];
        
        for (int i = 0; i < 2; i++)
        {
            // Normal
            vec3 vFixedNormal = vec3(position.xy, sqrt(r2 - dot(position.xy, position.xy)) * (2 * i - 1)) * (1 - 2 * i) / r;

            // Diffuse
            float lightness = 0.5 + 0.5 * dot(cvLightDirection, vFixedNormal);
            vec4 cDiffuse = vec4(baseColor.xyz * lightness, baseColor.a);

            // Specular
            vec3 reflected = reflect(-cvLightDirection, vFixedNormal);
            float specularFactor = pow(max(0.0, -reflected.z), fSpecularExponent);
            vec4 cSpecular = vec4(specularFactor * cSpecularColor, 0.0);
            
            ocColor[i] = cDiffuse.rgb + cSpecular.rgb;
        }
        
        gl_FragColor = vec4(mix(mix(ocColor[0], ocColor[1], (1. - baseColor.a) / (2. - baseColor.a)), background, (1. - baseColor.a) * (1. - baseColor.a)), 1);
    }
    else
        gl_FragColor = vec4(background, 1);
}
