#version 330 core
in vec2 screenCoord;

uniform vec2 screenSize;
uniform sampler2D randomMap;
uniform sampler2D envMap;

out vec4 FragColor;

//////////////////////////////////////////////////////////////////////////////
float randIdx = 0;
void seedcore2(vec2 screenCoord)
{
	float x = (screenCoord.x * screenSize.x);
	float y = (screenCoord.y * screenSize.y);
	
	randIdx = y * screenSize.x + x;
}

#define RAND_TEX_SIZE 1048

float randcore2()
{
	randIdx += 1 * RAND_TEX_SIZE;
	float u = mod(randIdx, RAND_TEX_SIZE) / RAND_TEX_SIZE;
	float v = floor(randIdx / RAND_TEX_SIZE) / RAND_TEX_SIZE;

	return texture(randomMap, vec2(u, v)).x;
}

void seed(vec2 screenCoord)
{
	seedcore2(screenCoord);
}

float rand()
{
	return randcore2();
}

vec2 rand2()
{
	return vec2(rand(), rand());
}

vec3 rand3()
{
	return vec3(rand(), rand(), rand());
}

vec4 rand4()
{
	return vec4(rand(), rand(), rand(), rand());
}

///////////////////////////////////////////////////////////////////////////////
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
};

struct World
{
	int objectCount;
	Sphere objects[10];
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

Ray CameraGetRay(Camera camera, vec2 uv)
{
	Ray ray = RayConstructor(camera.origin, 
		camera.lower_left_corner + 
		uv.x * camera.horizontal + 
		uv.y * camera.vertical);

	return ray;
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
bool SphereHit(Sphere sphere, Ray ray, float t_min, float t_max, inout HitRecord hitRecord)
{
	vec3 oc = ray.origin - sphere.center;
	
	float a = dot(ray.direction, ray.direction);
	float b = 2.0 * dot(oc, ray.direction);
	float c = dot(oc, oc) - sphere.radius * sphere.radius;

	float discriminant = b * b - 4 * a * c;
	if(discriminant > 0)
	{
		float temp = (-b - sqrt(discriminant)) / (2.0 * a);
		if(temp < t_max && temp> t_min)
		{
			hitRecord.t = temp;
			hitRecord.position = RayGetPointAt(ray, hitRecord.t);
			hitRecord.normal = (hitRecord.position - sphere.center) / sphere.radius;
			
			return true;
		}

		temp = (-b + sqrt(discriminant)) / (2.0 * a);
		if(temp < t_max && temp> t_min)
		{
			hitRecord.t = temp;
			hitRecord.position = RayGetPointAt(ray, hitRecord.t);
			hitRecord.normal = (hitRecord.position - sphere.center) / sphere.radius;
			
			return true;
		}
	}
	
	return false;
}

////////////////////////////////////////////////////////////////////////////////////
uniform Camera camera;
uniform World world;

bool WorldHit(Ray ray, float t_min, float t_max, inout HitRecord rec)
{
	HitRecord tempRec;
	float cloestSoFar = t_max;
	bool hitSomething = false;

	for(int i=0; i<world.objectCount; i++)
	{
		if(SphereHit(world.objects[i], ray, t_min, cloestSoFar, tempRec))
		{
			rec = tempRec;
			cloestSoFar = tempRec.t;

			hitSomething = true;
		}
	}

	return hitSomething;
}

vec3 PathTrace(Ray ray)
{
	HitRecord hitRecord;
	if(WorldHit(ray, 0.0, 1000000.0, hitRecord))
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
	seed(screenCoord);

	float u = screenCoord.x;
	float v = screenCoord.y;
	
	vec3 col = vec3(0.0, 0.0, 0.0);
	int ns = 100;
	for(int i=0; i<ns; i++)
	{
		Ray ray = CameraGetRay(camera, vec2(u, v) + rand2() / screenSize);
		col += PathTrace(ray);
	}
	col /= ns;

	FragColor.xyz = col;
	FragColor.w = 1.0;
}