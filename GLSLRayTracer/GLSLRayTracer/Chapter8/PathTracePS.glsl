#version 330 core
#define PI 3.14159265
#define RAYCAST_MAX 100000.0
in vec2 screenCoord;

uniform vec2 screenSize;

uniform sampler2D envMap;

uniform int sampleCount;

out vec4 FragColor;

///////////////////////////////////////////////////////////////////////////////
uint rng_state;

uint rand_lcg()
{
    // LCG values from Numerical Recipes
    rng_state = uint(1664525) * rng_state + uint(1013904223);
    return rng_state;
}

uint rand_xorshift()
{
    // Xorshift algorithm from George Marsaglia's paper
    rng_state ^= (rng_state << 13);
    rng_state ^= (rng_state >> 17);
    rng_state ^= (rng_state << 5);
    return rng_state;
}

void seedcore3(vec2 screenCoord)
{
	rng_state = uint(screenCoord.x * screenSize.x + screenCoord.y * screenSize.x * screenSize.y);
}

float randcore3()
{
	return float(rand_xorshift()) * (1.0 / 4294967296.0);
}

uint wang_seed;
uint wang_hash(uint seed)
{
    seed = (seed ^ uint(61)) ^ (seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> uint(4));
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> uint(15));
    return seed;
}

void seedcore4(vec2 screenCoord)
{
	wang_seed = uint(screenCoord.x * screenSize.x + screenCoord.y * screenSize.x * screenSize.y);
}

float randcore4()
{
	wang_seed = wang_hash(wang_seed);

	return float(wang_seed) * (1.0 / 4294967296.0);
}

void seed(vec2 screenCoord)
{
	seedcore4(screenCoord);
}

float rand()
{
	return randcore4();
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

vec3 random_in_unit_sphere()
{
	vec3 p;
	
	//do
	//{
		//p = 2.0 * rand3() - vec3(1, 1, 1);
	//}while(dot(p, p)>=1.0);
	//return p;

	//float phi		= rand() * 2.0 * PI;
	//float theta		= rand() * PI;
	//p.y = cos(theta);
	//p.x = sin(theta) * cos(phi);
	//p.z = sin(theta) * sin(phi);

	p = normalize(rand3());
	
	return p;
}

vec3 random_in_unit_disk()
{
	vec3 p;

	//float theta		= rand() * 2.0 * PI;
	//p.x = cos(theta);
	//p.y = sin(theta);
	//p.z = 0;

	p = normalize(vec3(rand(), rand(), 0));
	
	return p;
}

///////////////////////////////////////////////////////////////////////////////
struct Ray {
    vec3 origin;
    vec3 direction;
}; 

struct Camera 
{
	vec3 origin;
	vec3 target;
	vec3 up;
	float vfov;
	float aspect;
}; 

struct Sphere 
{
    vec3 center;
    float radius;
	int materialType;
	int material;
}; 

struct HitRecord
{
	float t;
	vec3 position;
	vec3 normal;
	
	int materialType;
	int material;
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
Ray CameraGetRay(Camera camera, vec2 offset)
{
	float halfHeight = tan(camera.vfov * PI / 180.0 / 2.0);
	float halfWidth = camera.aspect * halfHeight;

	vec3 w = normalize(camera.origin - camera.target);
	vec3 u = normalize(cross(camera.up, w));
	vec3 v = cross(w, u);

	vec3 lower_left_corner = camera.origin - halfWidth * u - halfHeight * v - w;
	vec3 horizontal = 2.0 * halfWidth * u;
	vec3 vertical = 2.0 * halfHeight * v;

	Ray ray;
	ray.origin = camera.origin;
	ray.direction = lower_left_corner + offset.x * horizontal + offset.y * vertical - camera.origin;
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
	if(discriminant>0)
	{
		float temp = (-b - sqrt(discriminant)) / (2.0 * a);
		if(temp < t_max && temp> t_min)
		{
			hitRecord.t = temp;
			hitRecord.position = RayGetPointAt(ray, hitRecord.t);
			hitRecord.normal = (hitRecord.position - sphere.center) / sphere.radius;
			
			hitRecord.materialType = sphere.materialType;
			hitRecord.material = sphere.material;

			return true;
		}

		temp = (-b + sqrt(discriminant)) / (2.0 * a);
		if(temp < t_max && temp> t_min)
		{
			hitRecord.t = temp;
			hitRecord.position = RayGetPointAt(ray, hitRecord.t);
			hitRecord.normal = (hitRecord.position - sphere.center) / sphere.radius;
			
			hitRecord.materialType = sphere.materialType;
			hitRecord.material = sphere.material;

			return true;
		}
	}
	
	return false;
}


////////////////////////////////////////////////////////////////////////////////////
#define MAT_LAMBERTIAN		0
#define MAT_METALLIC	1
#define MAT_DIELECTRIC	2
#define MAT_PBR			3

struct Lambertian
{
	vec3 albedo;
};

Lambertian LambertianConstructor(vec3 albedo)
{
	Lambertian lambertian;

	lambertian.albedo = albedo;

	return lambertian;
}

bool LambertianScatter(in Lambertian lambertian, in Ray incident, in HitRecord hitRecord, out Ray scattered, out vec3 attenuation)
{
	attenuation = lambertian.albedo;

	scattered.origin = hitRecord.position;
	scattered.direction = hitRecord.normal + random_in_unit_sphere();

	return true;
}

struct Metallic
{
	vec3 albedo;
	float roughness;
};

Metallic MetallicConstructor(vec3 albedo, float roughness)
{
	Metallic metallic;

	metallic.albedo = albedo;
	metallic.roughness = roughness;

	return metallic;
}

float schlick(float cosine, float ior)
{
	float r0 = (1 - ior) / (1 + ior);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}

vec3 reflect(in vec3 incident, in vec3 normal)
{
	return incident - 2 * dot(normal, incident) * normal;
}

bool refract(vec3 v, vec3 n, float ni_over_nt, out vec3 refracted)
{
	vec3 uv = normalize(v);
	float dt = dot(uv, n);
	float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1.0 - dt * dt);
	if (discriminant > 0)
	{
		refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	else
		return false;
}

bool MetallicScatter(in Metallic metallic, in Ray incident, in HitRecord hitRecord, out Ray scattered, out vec3 attenuation)
{
	attenuation = metallic.albedo;

	scattered.origin = hitRecord.position;
	scattered.direction = reflect(incident.direction, hitRecord.normal);

	return dot(scattered.direction, hitRecord.normal) > 0.0;
}

struct Dielectric
{
	vec3 albedo;
	float roughness;
	float ior;
};

Dielectric DielectricConstructor(vec3 albedo, float roughness, float ior)
{
	Dielectric dielectric;

	dielectric.albedo = albedo;
	dielectric.roughness = roughness;
	dielectric.ior = ior;

	return dielectric;
}

bool DielectricScatter1(in Dielectric dielectric, in Ray incident, in HitRecord hitRecord, out Ray scattered, out vec3 attenuation)
{
	attenuation = dielectric.albedo;
	vec3 reflected = reflect(incident.direction, hitRecord.normal);

	vec3 outward_normal;
	float ni_over_nt;
	if(dot(incident.direction, hitRecord.normal) > 0.0)// hit from inside
	{
		outward_normal = -hitRecord.normal;
		ni_over_nt = dielectric.ior;
	}
	else // hit from outside
	{
		outward_normal = hitRecord.normal;
		ni_over_nt = 1.0 / dielectric.ior;
	}

	vec3 refracted;
	if(refract(incident.direction, outward_normal, ni_over_nt, refracted))
	{
		scattered = Ray(hitRecord.position, refracted);

		return true;
	}
	else
	{
		scattered = Ray(hitRecord.position, reflected);

		return false;
	}
}

bool DielectricScatter2(in Dielectric dielectric, in Ray incident, in HitRecord hitRecord, out Ray scattered, out vec3 attenuation)
{
	attenuation = dielectric.albedo;
	vec3 reflected = reflect(incident.direction, hitRecord.normal);

	vec3 outward_normal;
	float ni_over_nt;
	float cosine;
	if(dot(incident.direction, hitRecord.normal) > 0.0)// hit from inside
	{
		outward_normal = -hitRecord.normal;
		ni_over_nt = dielectric.ior;
		cosine = dielectric.ior * dot(incident.direction, hitRecord.normal) / length(incident.direction); // incident angle
	}
	else // hit from outside
	{
		outward_normal = hitRecord.normal;
		ni_over_nt = 1.0 / dielectric.ior;
		cosine = -dot(incident.direction, hitRecord.normal) / length(incident.direction); // incident angle
	}

	float reflect_prob;
	vec3 refracted;
	if(refract(incident.direction, outward_normal, ni_over_nt, refracted))
	{
		reflect_prob = schlick(cosine, dielectric.ior);
	}
	else
	{
		reflect_prob = 1.0;
	}

	if(rand() < reflect_prob)
	{
		scattered = Ray(hitRecord.position, refracted);
	}
	else
	{
		scattered = Ray(hitRecord.position, refracted);
	}

	return true;
}

bool DielectricScatter(in Dielectric dielectric, in Ray incident, in HitRecord hitRecord, out Ray scattered, out vec3 attenuation)
{
	//return DielectricScatter1(dielectric, incident, hitRecord, scattered, attenuation);
	return DielectricScatter2(dielectric, incident, hitRecord, scattered, attenuation);
}

/////////////////////////////////////////////////////////////////////////////////
uniform World world;
uniform Camera camera;
uniform Lambertian lambertMaterials[4];
uniform Metallic metallicMaterials[4];
uniform Dielectric dielectricMaterials[4];

bool MaterialScatter(in Ray incident, in HitRecord hitRecord, out Ray scatter, out vec3 attenuation)
{
	if(hitRecord.materialType==MAT_LAMBERTIAN)
		return LambertianScatter(lambertMaterials[hitRecord.material], incident, hitRecord, scatter, attenuation);
	else if(hitRecord.materialType==MAT_METALLIC)
		return MetallicScatter(metallicMaterials[hitRecord.material], incident, hitRecord, scatter, attenuation);
	else if(hitRecord.materialType==MAT_DIELECTRIC)
		return DielectricScatter(dielectricMaterials[hitRecord.material], incident, hitRecord, scatter, attenuation);
	else
		return false;
}

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

vec3 GetEnvironmentColor(World world, Ray ray)
{
	vec3 unit_direction = normalize(ray.direction);
	float t = 0.5 * (unit_direction.y + 1.0);
	return vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;
	/*
	vec3 dir = normalize(ray.direction);
	float theta = acos(dir.y) / PI;
	float phi = (atan(dir.z, dir.x) + (PI / 2.0)) / PI;
	return texture(envMap, dir).xyz * envMapIntensity;
	*/
}

/*
vec3 PathTrace(Ray ray)
{
	HitRecord hitRecord;
	if(WorldHit(world, ray, 0.001, RAYCAST_MAX, hitRecord))
	{
		ray = RayConstructor(hitRecord.position, hitRecord.normal + random_in_unit_sphere());
		PathTrace(world, ray);
	}
	else
	{
		return GetEnvironmentColor(world, ray);
	}
}
*/

vec3 PathTrace(Ray ray, int depth)
{
	HitRecord hitRecord;

	vec3 current_attenuation = vec3(1.0, 1.0, 1.0);
	while(depth>0)
	{
		depth--;
		if(WorldHit(ray, 0.001, RAYCAST_MAX, hitRecord))
		{
			Ray scatterRay;
			vec3 attenuation;
			if(!MaterialScatter(ray, hitRecord, scatterRay, attenuation))
				break;
			
			current_attenuation *= attenuation;
			ray = scatterRay;
		}
		else
		{
			return current_attenuation * GetEnvironmentColor(world, ray);
		}
	}

	return vec3(0.0, 0.0, 0.0);
}

vec3 GammaCorrection(vec3 c)
{
	return pow(c, vec3(1.0 / 2.2));
}

vec3 InverseGammaCorrection(vec3 c)
{
	return pow(c, vec3(2.2));
}

void main()
{
	seed(screenCoord);

	vec3 col = vec3(0.0, 0.0, 0.0);
	int ns = sampleCount;
	for(int i=0; i<ns; i++)
	{
		Ray ray = CameraGetRay(camera, screenCoord + rand2() / screenSize);
		col += PathTrace(ray, 10);
	}
	col /= ns;

	//FragColor.xyz = GammaCorrection(col);
	FragColor.xyz = col;
	FragColor.w = 1.0;
}