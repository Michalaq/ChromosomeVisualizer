#version 330 core

uniform vec4 cColor;
uniform vec3 cSpecularColor;
uniform float fSpecularExponent;

in vec2 position;

void main() {
    if (dot(position, position) < 1.)
    {
        const vec3 cvLightDirection = normalize(vec3(-1., 1., -2.));
        vec4 baseColor = cColor;
        
        vec3 ocColor[2];
        
        for (int i = 0; i < 2; i++)
        {
            // Normal
            vec3 vFixedNormal = vec3(position.xy, sqrt(1. - dot(position.xy, position.xy)) * (2 * i - 1));

            // Diffuse
            float lightness = 0.5 + 0.5 * dot(cvLightDirection, vFixedNormal);
            vec4 cDiffuse = vec4(baseColor.xyz * lightness, baseColor.a);

            // Specular
            vec3 reflected = reflect(-cvLightDirection, vFixedNormal);
            float specularFactor = pow(max(0.0, -reflected.z), fSpecularExponent);
            vec4 cSpecular = vec4(specularFactor * cSpecularColor, 0.0);
            
            ocColor[i] = cDiffuse.rgb + cSpecular.rgb;
        }
        
        gl_FragColor = vec4((ocColor[0] + (1. - baseColor.a) * ocColor[1]) / (2. - baseColor.a), baseColor.a * (2. - baseColor.a));
    }
    else
        discard;
}
