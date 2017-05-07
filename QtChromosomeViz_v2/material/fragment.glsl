#version 330 core

uniform vec4 cColor;
uniform vec3 cSpecularColor;
uniform float fSpecularExponent;

in vec2 position;

void main() {
    if (dot(position, position) < 1.)
    {
        const vec3 cvLightDirection = vec3(0.7, 0.0, 0.7);
        vec4 baseColor = cColor;
        
        // Normal
        vec3 vFixedNormal = vec3(position.xy, sqrt(1. - dot(position.xy, position.xy)));

        // Diffuse
        float lightness = 0.5 + 0.5 * dot(cvLightDirection, vFixedNormal);
        vec4 cDiffuse = vec4(baseColor.xyz * lightness, baseColor.a);

        // Specular
        vec3 reflected = reflect(-cvLightDirection, vFixedNormal);
        float specularFactor = pow(max(0.0, reflected.z), fSpecularExponent);
        vec4 cSpecular = vec4(specularFactor * cSpecularColor, 0.0);
        
        gl_FragColor = vec4(cDiffuse.rgb + cSpecular.rgb, baseColor.a);
    }
    else
        gl_FragColor = vec4(0);
}
