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

struct Camera 
{
    vec3 lower_left_corner;
    vec3 horizontal;
	vec3 vertical;
	vec3 origin;
}; 

struct Sphere 
{
    vec3 center;
    float radius;
}; 

struct HitRecord
{
	float t;
	vec3 position;
	vec3 normal;
	bool hit;
};

////////////////////////////////////////////////////////////////////////////////////
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

float RayHitSphere(Ray ray, Sphere sphere)
{
	vec3 oc = ray.origin - sphere.center;
	
	float a = dot(ray.direction, ray.direction);
	float b = 2.0 * dot(oc, ray.direction);
	float c = dot(oc, oc) - sphere.radius * sphere.radius;

	float discriminant = b * b - 4 * a * c;
	if(discriminant<0)
		return -1.0;
	else
		return (-b - sqrt(discriminant)) / (2.0 * a);
}

////////////////////////////////////////////////////////////////////////////////////
Camera CameraConstructor(vec3 lower_left_corner, vec3 horizontal, vec3 vertical, vec3 origin)
{
	Camera camera;

	camera.lower_left_corner = lower_left_corner;
	camera.horizontal = horizontal;
	camera.vertical = vertical;
	camera.origin = origin;

	return camera;
}

////////////////////////////////////////////////////////////////////////////////////
Sphere SphereConstructor(vec3 center, float radius)
{
	Sphere sphere;

	sphere.center = center;
	sphere.radius = radius;

	return sphere;
}

////////////////////////////////////////////////////////////////////////////////////
HitRecord SphereHit(Sphere sphere, Ray ray, float t_min, float t_max)
{
	HitRecord hitRecord;

	vec3 oc = ray.origin - sphere.center;
	
	float a = dot(ray.direction, ray.direction);
	float b = dot(oc, ray.direction);
	float c = dot(oc, oc) - sphere.radius * sphere.radius;

	float discriminant = b * b - a * c;
	if(discriminant>0)
	{
		float temp = (-b - sqrt(discriminant)) / (a);
		if(temp < t_max && temp> t_min)
		{
			hitRecord.hit = true;

			hitRecord.t = temp;
			hitRecord.position = RayGetPointAt(ray, hitRecord.t);
			hitRecord.normal = (hitRecord.position - sphere.center) / sphere.radius;
			return hitRecord;
		}

		temp = (-b + sqrt(discriminant)) / (a);
		if(temp < t_max && temp> t_min)
		{
			hitRecord.hit = true;

			hitRecord.t = temp;
			hitRecord.position = RayGetPointAt(ray, hitRecord.t);
			hitRecord.normal = (hitRecord.position - sphere.center) / sphere.radius;
			return hitRecord;
		}
	}
	
	hitRecord.hit = false;
	return hitRecord;
}

struct World
{
	int objectCount;
	Sphere objects[10];
};

World WorldConstructor()
{
	World world;

	world.objectCount = 2;
	world.objects[0] = SphereConstructor(vec3(0.0, 0.0, -1.0), 0.5);
	world.objects[1] = SphereConstructor(vec3(0.0, -100.5, -1.0), 100.0);

	return world;
}

HitRecord WorldHit(World world, Ray ray, float t_min, float t_max)
{
	HitRecord tempRec;
	HitRecord rec;
	rec.hit = false;
	float cloestSoFar = t_max;

	for(int i=0; i<world.objectCount; i++)
	{
		tempRec = SphereHit(world.objects[i], ray, t_min, cloestSoFar);
		if(tempRec.hit)
		{
			rec = tempRec;
			cloestSoFar = tempRec.t;
		}
	}

	return rec;
}

vec3 WorldTrace(World world, Ray ray)
{
	HitRecord hitRecord = WorldHit(world, ray, 0.0, 1000000.0);
	if(hitRecord.hit)
	{
		return 0.5 * vec3(hitRecord.normal.x+1, hitRecord.normal.y+1, hitRecord.normal.z+1);
	}
	else
	{
		vec3 unit_direction = normalize(ray.direction);
		float t = 0.5 * (unit_direction.y + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

void main()
{
	World world = WorldConstructor();
	Camera camera = CameraConstructor(vec3(-2.0, -1.0, -1.0), vec3(4.0, 0.0, 0.0), vec3(0.0, 2.0, 0.0), vec3(0.0, 0.0, 0.0));
	Ray ray = RayConstructor(camera.origin, camera.lower_left_corner + screenCoord.x * camera.horizontal + screenCoord.y * camera.vertical);

	FragColor.xyz = WorldTrace(world, ray);
	FragColor.w = 1.0;
}