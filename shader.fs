#version 330 core

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;

out vec4 color;

uniform sampler2D Texture0;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float specularStrength;

void main()
{
    color = texture(Texture0, TexCoord);

	float ambientStrength = 0.4;
    vec3 ambient = ambientStrength * lightColor;

  	vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;
            
    vec3 result = (ambient + diffuse + specular) * vec3(color);
    color = vec4(result, 1.0);
}
