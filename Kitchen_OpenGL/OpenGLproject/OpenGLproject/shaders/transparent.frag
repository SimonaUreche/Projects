#version 410 core

out vec4 fragColor;

uniform float alpha;

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 passTexture;

uniform sampler2D diffuseTexture;
uniform	vec3 lightDir;
uniform	vec3 lightColor;     

vec3 ambient;
uniform float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;
vec3 viewPos = vec3(0.0f);

void computeDirLight() {
    ambient = vec3(0.0);
    diffuse = vec3(0.0);
    specular = vec3(0.0);

    vec3 norm = normalize(fragNormal);
    vec3 lightDirNorm = normalize(lightDir);
    vec3 viewDirNorm = normalize(viewPos - fragPosition.xyz);

    ambient = ambientStrength * lightColor;
    diffuse = max(dot(norm, lightDirNorm), 0.0f) * lightColor;
    vec3 reflectDir = normalize(reflect(-lightDirNorm, norm));
    float specCoeff = pow(max(dot(viewDirNorm, reflectDir), 0.0f), shininess);
    specular = specularStrength * specCoeff * lightColor;
}

void main() {
	vec3 lighting = vec3(0.0);
	computeDirLight();
	
	lighting += ambient + diffuse + specular; 
	
	
	vec4 texColor = texture(diffuseTexture, passTexture);
	vec3 finalColor = texColor.rgb * lighting;
    fragColor = vec4(finalColor, texColor.a * alpha);
}
