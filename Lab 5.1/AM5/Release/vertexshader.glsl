#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 textureCoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 vertexNormal;
out vec3 fragPos;
out vec2 texCoords;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);

	//vertexNormal = normal;
	fragPos = vec3(modelMatrix * vec4(position, 1.0f));

	texCoords = textureCoords.xy;
}