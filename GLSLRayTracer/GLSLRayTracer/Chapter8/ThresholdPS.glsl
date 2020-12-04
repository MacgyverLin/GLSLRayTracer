#version 330 core
in vec2 screenCoord;

uniform vec2 screenSize;
uniform sampler2D texture0;
uniform float threshold;

out vec4 FragColor;

void main()
{
    vec3 col = texture(texture0, screenCoord).xyz;
    float brightness = dot(col, vec3(0.2126, 0.7152, 0.0722));
    if(brightness>threshold)
    {
        FragColor = vec4(col, 1.0);
    }
    else
    {
        FragColor = vec4(0.0);
    }
}