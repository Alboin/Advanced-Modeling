#version 330 core

in vec3 vertexNormal;
in vec3 fragPos;
in vec4 gl_FragCoord;
in vec2 texCoords;

out vec4 color;

uniform sampler2D ourTexture;
uniform vec3 cameraPos;

void main()
{
	float near = 0.1f;
	float far = 5.0f;

	//compute a normal without interpolation (as in flat shading).
	vec3 normal = normalize(cross(dFdx(fragPos), dFdy(fragPos)));

	//perform some basic shading
	//vec3 red = vec3(1.0f, 0.3f, 0.3f);
	vec3 red = texture(ourTexture, texCoords).xyz;
	color = vec4(max(dot(normalize(cameraPos), normal), 0.0f) * red, 1.0f);

}