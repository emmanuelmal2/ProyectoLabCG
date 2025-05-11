#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 ex_N;

uniform sampler2D texture_diffuse1;

void main()
{    
    vec4 texel = texture(texture_diffuse1, TexCoords);

    if (texel.a < 0.1)
        discard;

    vec4 ambientColor = vec4(1.0, 1.0, 1.0, 1.0);
    FragColor = ambientColor * texel;
}
