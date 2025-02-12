#version 410 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;//normakla folosita pt calculul iluminarii
layout(location = 2) in vec2 textcoord;

out vec3 fragNormal;
out vec3 fragPosition;
out vec2 passTexture; 
out vec4 fragPosLightSpace; //pozitia varfului in spatiul luminii ~ umbre
out vec4 fragmentPosEyeSpace; //pozitia varfului in spatiul camerei ~ umbre

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform	mat3 normalMatrix;
uniform mat4 lightSpaceTrMatrix; //matricea de transformare in spatiul luminii


void main() {
	//1.pozitia vf ->spatiul obiectului -> spatiul lumii -> spatiul luminii
	fragPosLightSpace = lightSpaceTrMatrix * model * vec4(vertexPosition, 1.0f);//folosit pt. a a calcula umbrele
	
	fragmentPosEyeSpace = view * model * vec4(vertexPosition, 1.0f); //pozitia varfului in spatiul camerei ~ ceata

	fragPosition = vec3(model * vec4(vertexPosition, 1.0)); 
    fragNormal = normalize(normalMatrix * vertexNormal); //pt a se adapta la operatiile aplicate obiectului este transformat in normalMatrix
	
    passTexture = textcoord; // Transmitem coordonatele texturii mai departe
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}
