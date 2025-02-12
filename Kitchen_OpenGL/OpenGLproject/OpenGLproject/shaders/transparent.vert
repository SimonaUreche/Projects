#version 410 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 textcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform	mat3 normalMatrix;

out vec3 fragNormal;
out vec3 fragPosition;
out vec2 passTexture; 


void main() {
	

	fragPosition = vec3(model * vec4(vertexPosition, 1.0)); 
    fragNormal = normalize(normalMatrix * vertexNormal);
	
    passTexture = textcoord; // Transmitem coordonatele texturii mai departe
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}
