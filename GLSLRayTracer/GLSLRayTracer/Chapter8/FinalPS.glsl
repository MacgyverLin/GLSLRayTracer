#version 330 core
in vec2 screenCoord;

uniform vec2 screenSize;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float bloomAmount;

out vec4 FragColor;

void main()
{
	FragColor = texture(texture0, screenCoord) + bloomAmount * texture(texture1, screenCoord);
	FragColor.w = 1;
}