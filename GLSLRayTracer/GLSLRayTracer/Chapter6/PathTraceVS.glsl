#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 screenCoord;

void main()
{
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	screenCoord = (vec2(aPos.x, aPos.y) + 1.0) / 2.0;
}