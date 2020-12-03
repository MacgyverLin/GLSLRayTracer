#version 330 core
in vec2 screenCoord;

uniform vec2 screenSize;

uniform sampler2D frameBufferTexture;

uniform float A;

out vec4 FragColor;

void main()
{
	FragColor = (texture(frameBufferTexture, screenCoord)) / (A+1.0);
}