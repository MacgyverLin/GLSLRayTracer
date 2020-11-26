#version 330 core

in vec2 screenCoord;

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform vec2 screenSize;

out vec4 FragColor;

struct Ray {
    vec3 origin;
    vec3 direction;
}; 

Ray Ray_Constructor(vec3 origin, vec3 direction)
{
	Ray ray;
	ray.origin = origin;
	ray.direction = direction;

	return ray;
}

vec3 Ray_PointAt(Ray ray, float t)
{
	return ray.origin + t * ray.direction;
}
//////////////////////////////////////////
struct Camera 
{
    vec3 lower_left_corner;
    vec3 horizontal;
	vec3 vertical;
	vec3 origin;
}; 

Camera Camera_constructor(vec3 lower_left_corner, vec3 horizontal, vec3 vertical, vec3 origin)
{
	Camera camera;

	camera.lower_left_corner = lower_left_corner;
	camera.horizontal = horizontal;
	camera.vertical = vertical;
	camera.origin = origin;

	return camera;
}

vec3 Trace(Ray r)
{
	vec3 unit_direction = normalize(r.direction);
	float t = 0.5 * unit_direction.y + 1.0;
	return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

void main()
{
	Camera camera = Camera_constructor(vec3(-2.0, -1.0, -1.0), vec3(4.0, 0.0, 0.0), vec3(0.0, 2.0, 0.0), vec3(0.0, 0.0, 0.0));
	Ray ray = Ray_Constructor(camera.origin, camera.lower_left_corner + screenCoord.x * camera.horizontal + screenCoord.y * camera.vertical);

	FragColor.xyz = Trace(ray);
	FragColor.w = 1.0;
}