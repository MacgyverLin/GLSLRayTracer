#version 330 core
in vec2 screenCoord;

uniform vec2 screenSize;
uniform sampler2D texture0;
uniform bool horizontal;

out vec4 FragColor;

float gussianWeights2[] =  float[](0.2350, 0.1112, 0.0941, 0.0713, 0.0483, 0.0293, 0.0159, 0.0077, 0.0047);
float gussianWeights[] =  float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
vec3 guassian(bool horizontal)
{
    vec2 offset = 1.0 / screenSize;

    vec3 col = gussianWeights2[0] * texture(texture0, screenCoord).xyz;
    if(horizontal)
    {
        for(int i=1; i<9; i++)
        {
            col += gussianWeights2[i] * texture(texture0, screenCoord + vec2(offset.x * i, 0.0)).xyz;
            col += gussianWeights2[i] * texture(texture0, screenCoord - vec2(offset.x * i, 0.0)).xyz;
        }
    }
    else
    {
        for(int i=1; i<9; i++)
        {
            col += gussianWeights2[i] * texture(texture0, screenCoord + vec2(0.0, offset.y * i)).xyz;
            col += gussianWeights2[i] * texture(texture0, screenCoord - vec2(0.0, offset.y * i)).xyz;
        }
    }

    return col;
}

void main()
{
	FragColor.xyz = guassian(horizontal);
    FragColor.w = 1.0;
}