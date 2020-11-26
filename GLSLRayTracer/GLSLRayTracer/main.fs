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

Ray RayConstructor(vec3 origin, vec3 direction)
{
	Ray ray;
	ray.origin = origin;
	ray.direction = direction;

	return ray;
}

vec3 RayGetPointAt(Ray ray, float t)
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

Camera CameraConstructor(vec3 lower_left_corner, vec3 horizontal, vec3 vertical, vec3 origin)
{
	Camera camera;

	camera.lower_left_corner = lower_left_corner;
	camera.horizontal = horizontal;
	camera.vertical = vertical;
	camera.origin = origin;

	return camera;
}

//////////////////////////////////////////
struct Sphere 
{
    vec3 center;
    float radius;
}; 

Sphere SphereConstructor(vec3 center, float radius)
{
	Sphere sphere;

	sphere.center = center;
	sphere.radius = radius;

	return sphere;
}

bool RayHitSphere(Sphere sphere, Ray ray)
{
	vec3 oc = ray.origin - sphere.center;
	
	float a = dot(ray.direction, ray.direction);
	float b = 2.0 * dot(oc, ray.direction);
	float c = dot(oc, oc) - sphere.radius * sphere.radius;

	float discriminant = b * b - 4 * a * c;

	return discriminant > 0.0;
}

vec3 WorldTrace(Ray ray)
{
	Sphere sphere = SphereConstructor(vec3(0.0, 0.0, -1.0), 0.5);
	if(RayHitSphere(sphere, ray))
		return vec3(1, 0, 0);

	vec3 unit_direction = normalize(ray.direction);
	float t = 0.5 * (unit_direction.y + 1.0);
	return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

void main()
{
	Camera camera = CameraConstructor(vec3(-2.0, -1.0, -1.0), vec3(4.0, 0.0, 0.0), vec3(0.0, 2.0, 0.0), vec3(0.0, 0.0, 0.0));
	Ray ray = RayConstructor(camera.origin, camera.lower_left_corner + screenCoord.x * camera.horizontal + screenCoord.y * camera.vertical);

	FragColor.xyz = WorldTrace(ray);
	FragColor.w = 1.0;
}