#version 330 core
in vec2 screenCoord;

uniform vec2 screenSize;
uniform sampler2D randomMap;
uniform sampler2D envMap;

out vec4 FragColor;

void main()
{
	FragColor = vec4(screenCoord.x, screenCoord.y, 0.2, 1.0);
}