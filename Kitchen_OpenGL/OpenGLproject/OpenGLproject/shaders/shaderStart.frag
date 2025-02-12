#version 410 core

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 passTexture;
in vec4 fragPosLightSpace; //pozitia fragmentului in spatiul luminii ~ umbre
in vec4 fragmentPosEyeSpace; //pozitia fragmentului in spatiul camerei ~ ceata

out vec4 fragmentColour;

uniform	vec3 lightDir;
uniform	vec3 lightColor;     

uniform sampler2D diffuseTexture; //textura de baza a suprafetei
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;//textura ce contine harde de adancime pt. umbre

vec3 ambient;
uniform float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

////CONTROL LUMINI
uniform bool isLightEnabled;
uniform bool isPointLightEnabled;
uniform bool isShadowEnabled;
uniform float alpha; 

vec3 viewPos = vec3(0.0f);

float computeFog() //factor e ceata bazat pe distanta fragmentului fata de camera
{
	 float fogDensity = 0.06f;//0.1f;

	 float fragmentDistance = length(fragmentPosEyeSpace);
	 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2)); //atenuarea culorii

	 return clamp(fogFactor, 0.0f, 1.0f);
}

//////////////pt. lumini punctiforme
struct PointLight {
    vec3 position;   
    vec3 color;      
    float constant;  
    float linear;    
    float quadratic; 
};

uniform PointLight pointLights[3];

void computePointLight(int index, out vec3 ambient, out vec3 diffuse, out vec3 specular) {
    ambient = vec3(0.0);
    diffuse = vec3(0.0);
    specular = vec3(0.0);

    vec3 norm = normalize(fragNormal);
    vec3 lightDirNorm = normalize(pointLights[index].position - fragPosition); 
    vec3 viewDirNorm = normalize(viewPos - fragPosition);

    float distance = length(pointLights[index].position - fragPosition);
    float attenuation = 1.0 / (pointLights[index].constant + 
                               pointLights[index].linear * distance + 
                               pointLights[index].quadratic * (distance * distance));

    // Calcul lumini
    vec3 lightColor = pointLights[index].color;
    ambient = ambientStrength * lightColor; 
    diffuse = max(dot(norm, lightDirNorm), 0.0f) * lightColor;
    vec3 halfVector = normalize(lightDirNorm + viewDirNorm);
    float specCoeff = pow(max(dot(viewDirNorm, halfVector), 0.0f), shininess);
    specular = specularStrength * specCoeff * lightColor;

    ambient *= attenuation; //+atenuare pe lumini
    diffuse *= attenuation;
    specular *= attenuation;
}


void computeDirLight() {//lumina sursa mare
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

float computeShadow() { //calculeaza umbrele folosind harta de adancime
	//1.Transfroma pozitia fragmentului din spatiul lumii in coordonatele de textura normalizata
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; 
    normalizedCoords = normalizedCoords * 0.5 + 0.5;

    // Dacă fragmentul este în afara hărții de umbre, nu e umbrit
    if (normalizedCoords.z > 1.0f ) {
        return 0.0f;
    }

	//comparam adancimea fragmentului cu cea mai apropiata adancime din harta
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r; // Valoarea din shadow map
    float currentDepth = normalizedCoords.z;

    // Compensarea (bias) pentru eliminarea artefactelor din umbre
    float bias = max(0.05 * (1.0f - dot(normalize(fragNormal), lightDir)), 0.005f);
    
    // Dacă poziția fragmentului este mai adâncă decât cea mai apropiată valoare din shadow map, este în umbră
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f; //cu umbra sau fara
	
	return shadow;
}

void main() {

    vec4 texColor = texture(diffuseTexture, passTexture);
	
	if (texColor.a < 0.1) {
        discard; // Elimină fragmentul
    }
	
    vec3 lighting = vec3(0.0);

	if (isLightEnabled || isShadowEnabled) {
		// Calculăm lumina direcțională
		computeDirLight();

		if (isShadowEnabled) {
			// Calculăm umbra dacă este activată
			float shadow = computeShadow();
			lighting += min((ambient + diffuse * (1.0f - shadow)) + specular * (1.0f - shadow), 1.0f);
		} else {
			// Dacă umbrele nu sunt active, aplicăm lumina normală
			lighting += ambient + diffuse + specular;
		}
	}

    if (isPointLightEnabled) {
		vec3 ambientTotal = vec3(0.0);
		vec3 diffuseTotal = vec3(0.0);
		vec3 specularTotal = vec3(0.0);
		for (int i = 0; i < 3; i++) {
			vec3 ambient, diffuse, specular;
			computePointLight(i, ambient, diffuse, specular);
			ambientTotal += ambient;
			diffuseTotal += diffuse;
			specularTotal += specular;
		}

		lighting += ambientTotal + diffuseTotal + specularTotal;
    }

    if (!isLightEnabled && !isPointLightEnabled && !isShadowEnabled) {
        lighting = ambientStrength * vec3(1.0, 1.0, 1.0);
    }
	
	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f); 
	
    //fragmentColour = vec4(lighting, 1.0) * texColor;
	fragmentColour = mix(fogColor, vec4(lighting, 1.0) * texColor, fogFactor);//cu ceata

}
