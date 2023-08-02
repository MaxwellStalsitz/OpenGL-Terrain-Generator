#version 330 core
out vec4 FragColor; //final color

in vec3 modelColor;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 cameraPos;

uniform bool isSpecular;
uniform float specularStrength;

uniform float sunIntensity; 
uniform float ambientStrength; 
uniform vec3 sunDir; 
uniform vec3 sunColor; 
uniform vec3 skyColor; 

void main()
{
    vec3 vertexColor = modelColor;

    vec2 uv = gl_FragCoord.xy/vec2(1920,1080).xy;
    vec3 norm = normalize(Normal);

    vec3 sunDirection;
    sunDirection = normalize(sunDir);

    //sun lighting
    float sunDiffuse = clamp(dot(norm, sunDirection), 0.0, 1.0);
    vec3 diffuse = vertexColor * sunIntensity * sunColor * sunDiffuse;

    //sky ighting
    float skyDiffuse = clamp(0.5 + 0.5 * dot(norm, vec3(0.0, 1.0, 0.0)), 0.0, 1.0);
    diffuse += vertexColor * skyColor * skyDiffuse;

    //ambient lighting
    vec3 ambient = ambientStrength * vertexColor;

    //specular lighting
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 reflectDir = reflect(-sunDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * sunColor;

    if (!isSpecular){
        specular = vec3(0);
    }
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}