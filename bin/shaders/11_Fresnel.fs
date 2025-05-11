#version 330 core

in vec3 WorldPos;
in vec3 WorldNormal;
in vec3 viewDir;
in vec2 TexCoords;

out vec4 FragColor;

// Texturas
uniform sampler2D diffuseMap;
uniform samplerCube skybox;

// Parámetros Fresnel
uniform float _Bias;
uniform float _Scale;
uniform float _Power;
uniform float uAlpha; 

void main()
{
    // Calcular dirección de refracción
    vec3 I = normalize(viewDir);
    vec3 N = normalize(WorldNormal);
    vec3 refractDir = refract(I, N, 1.0 / 1.003); // aire

    // Obtener color base desde la textura del objeto
    vec3 baseColor = texture(diffuseMap, TexCoords).rgb;

    // Obtener color de refracción desde el skybox
    vec3 refractedColor = texture(skybox, refractDir).rgb;

    // Calcular efecto Fresnel
    float fresnelFactor = _Bias + _Scale * pow(1.0 - dot(I, N), _Power);

    // Mezclar color base con color de refracción
    vec3 finalColor = mix(baseColor, refractedColor, fresnelFactor);

    FragColor = vec4(finalColor, uAlpha); //transparencia
}
