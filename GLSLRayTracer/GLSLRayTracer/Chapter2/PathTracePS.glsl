#version 330 core
in vec2 screenCoord;

uniform vec2 screenSize;

uniform sampler2D envMap;

out vec4 FragColor;

struct Ray {
    vec3 origin;
    vec3 direction;
}; 

struct Camera 
{
    vec3 lower_left_corner;
    vec3 horizontal;
	vec3 vertical;
	vec3 origin;
}; 

vec3 PathTrace(Ray ray)
{
	vec3 unit_direction = normalize(ray.direction);
	float t = 0.5 * (unit_direction.y + 1.0);
	return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

uniform Camera camera;

void main()
{
	float u = screenCoord.x;
	float v = screenCoord.y;
	
	Ray ray;
	ray.origin = camera.origin;
	ray.direction = camera.lower_left_corner + 
						u * camera.horizontal + 
						v * camera.vertical - camera.origin;

	FragColor.xyz = PathTrace(ray);
	FragColor.w = 1.0;
}