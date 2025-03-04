#version 410 core

layout(location = 0) in vec3 vertexPosition;

uniform mat4 lightSpaceTrMatrix;
uniform mat4 model;

void main() 
{
    gl_Position = lightSpaceTrMatrix * model * vec4(vertexPosition, 1.0);
}
