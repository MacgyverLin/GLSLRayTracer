#version 330 core
in vec2 screenCoord;

uniform vec2 screenSize;
uniform sampler2D frameBufferTexture;

uniform bool horizontal;
uniform bool useGussian;
uniform bool useThreshold;
uniform float threshold;

out vec4 FragColor;

vec3 tex0()
{
	return texture(frameBufferTexture, screenCoord).xyz;
}

vec3 negative(vec3 c)
{
    return vec3(1.0) - c;
}

vec3 greyscale(vec3 c)
{
    return vec3(c.r*0.3 + 0.6*c.g + 0.1*c.b);
}

vec3 filter(float kernel[9])
{
    vec2 pixelOffset = vec2(1.0 / screenSize);

    vec3 outc = vec3(0.0);

    vec2 offsetxy = vec2(-pixelOffset.x * 1, -pixelOffset.y * 1);
    int i = 0;
    for(int y=0; y<3; y++)
    {
        for(int x=0; x<3; x++)
        {
            outc += kernel[i++] * texture(frameBufferTexture, screenCoord + offsetxy).xyz;
        
            offsetxy.x += pixelOffset.x;
        }
        offsetxy.y += pixelOffset.y;
    }

    return outc;
}

float embross3x3[] = float[]
(
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1
);

float blur3x3[] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

float edgeDetection3x3[] = float[]
(
    1, 1, 1,
    1, -8, 1,
    1, 1, 1
);

void guassianKernel(out float kernel[9], float sigma)
{
    int i=0;

    float yy = -1;
    for(int y=0; y<3; y++)
    {
        float xx = -1;
        for(int x=0; x<3; x++)
        {
            float gxy = 1.0 / (2.0 * 3.14 * sigma * sigma) * exp(-(xx*xx+yy*yy)/(2*sigma * sigma));

            kernel[i++] = gxy;

            xx += 1;
        }
        yy += 1;
    }
}

float gussianWeights2[] =  float[](0.2350, 0.1112, 0.0941, 0.0713, 0.0483, 0.0293, 0.0159, 0.0077, 0.0047);
float gussianWeights[] =  float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
vec3 guassian(bool horizontal)
{
    vec2 offset = 1.0 / screenSize;

    vec3 col = gussianWeights2[0] * texture(frameBufferTexture, screenCoord).xyz;
    if(horizontal)
    {
        for(int i=1; i<9; i++)
        {
            col += gussianWeights2[i] * texture(frameBufferTexture, screenCoord + vec2(offset.x * i, 0.0)).xyz;
            col += gussianWeights2[i] * texture(frameBufferTexture, screenCoord - vec2(offset.x * i, 0.0)).xyz;
        }
    }
    else
    {
        for(int i=1; i<9; i++)
        {
            col += gussianWeights2[i] * texture(frameBufferTexture, screenCoord + vec2(0.0, offset.y * i)).xyz;
            col += gussianWeights2[i] * texture(frameBufferTexture, screenCoord - vec2(0.0, offset.y * i)).xyz;
        }
    }

    return col;
}

void main()
{
	// FragColor = vec4(negative(tex0()), 1.0);
	// FragColor = vec4(greyscale(tex0()), 1.0);
    // FragColor = vec4(filter(embross3x3, 3, 3), 1.0);
    // FragColor = vec4(filter(blur3x3, 3, 3), 1.0);
    // FragColor = vec4(filter(edgeDetection3x3, 3, 3), 1.0);
    // float guassian[9];
    // guassianKernel(guassian, 1.0);
    // FragColor = vec4(filter(guassian), 1.0);
    
    vec4 finalcolor = vec4(1.0, 0.0 ,1.0, 1.0);
    if(useThreshold)
    {
        vec3 col = tex0();
        float brightness = dot(col, vec3(0.2126, 0.7152, 0.0722));
        if(brightness>threshold)
        {
            finalcolor = vec4(col, 1.0);
        }
        else
        {
            finalcolor = vec4(0.0);
        }
    }
    else
    {
        if(useGussian)
        {
            finalcolor = vec4(guassian(horizontal), 1.0);
        }
        else
        {
            finalcolor = vec4(tex0(), 1.0);
        }
    }

    FragColor = finalcolor;
}