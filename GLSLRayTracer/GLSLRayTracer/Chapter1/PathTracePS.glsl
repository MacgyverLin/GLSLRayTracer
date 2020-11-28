#version 330 core
in vec2 screenCoord;

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D envMap;
uniform vec2 screenSize;

out vec4 FragColor;

void main()
{
	FragColor = vec4(screenCoord.x, screenCoord.y, 0.2, 1.0);
}