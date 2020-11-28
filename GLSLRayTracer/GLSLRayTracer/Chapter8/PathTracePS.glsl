#version 330 core
#define PI 3.14159265
#define RAYCAST_MAX 100000.0

in vec2 screenCoord;

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D envMap;
uniform vec2 screenSize;

uniform vec3 cameraPos;
uniform vec3 cameraTarget;
uniform vec3 cameraUp;
//uniform vec3 cameraPos = vec3(0.0, 0.0, 0.0);
//uniform vec3 cameraTarget = vec3(0.0, 0.0, -1.0);
//uniform vec3 cameraUp = vec3(0.0, 1.0, 0.0);

out vec4 FragColor;

//////////////////////////////////////////////////////////////////////////////
uint m_u = uint(521288629);
uint m_v = uint(362436069);

uint GetUintCore(inout uint u, inout uint v)
{
	v = uint(36969) * (v & uint(65535)) + (v >> 16);
	u = uint(18000) * (u & uint(65535)) + (u >> 16);
	return (v << 16) + u;
}

float GetUniformCore(inout uint u, inout uint v)
{
	uint z = GetUintCore(u, v);
	
	return float(z) / uint(4294967295);
}

float GetUniform()
{
	return GetUniformCore(m_u, m_v);
}

uint GetUint()
{
	return GetUintCore(m_u, m_v);
}

float rand()
{
	return GetUniform();
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
	// vec3 p;
	// 
	// do
	// {
	// 	p = 2.0 * rand3() - vec3(1, 1, 1);
	// }while(dot(p, p)>=1.0);
	// return p;

	vec3 p;
	
	float theta = rand() * 2.0 * PI;
	float phi   = rand() * PI;
	p.y = cos(phi);
	p.x = sin(phi) * cos(theta);
	p.z = sin(phi) * sin(theta);
	
	return p;
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

struct LambertianMaterial
{
	vec3 albedo;
};

struct MetallicMaterial
{
	vec3 albedo;
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


//////////////////////////////////////////////-//////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////////
Camera CameraConstructor(vec3 lower_left_corner, vec3 horizontal, vec3 vertical, vec3 origin)
{
	Camera camera;

	camera.origin = origin;
	camera.lower_left_corner = lower_left_corner;
	camera.horizontal = horizontal;
	camera.vertical = vertical;

	return camera;
}

Camera CameraSet(vec3 eye, vec3 target, vec3 up, float vfov, float aspect)
{
	Camera camera;

	float halfHeight = tan(vfov * PI / 180.0 / 2.0);
	float halfWidth = halfHeight * aspect;

	// right hand
	vec3 zAxis = normalize(eye - target);
	vec3 xAxis = normalize(cross(up, zAxis));
	vec3 yAxis = normalize(cross(zAxis, xAxis));

	camera.origin = eye;
	camera.horizontal = (2.0 * halfWidth) * xAxis;
	camera.vertical = (2.0 * halfHeight) * yAxis;
	camera.lower_left_corner = camera.origin - camera.horizontal / 2.0 - camera.vertical / 2.0 - zAxis;

	return camera;
}

Ray CameraGetRay(Camera camera, vec2 uv)
{
	Ray ray = RayConstructor(camera.origin, 
		camera.lower_left_corner + uv.x * camera.horizontal + uv.y * camera.vertical - camera.origin);

	return ray;
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
		cosine = dot(incident.direction, hitRecord.normal) / length(incident.direction); // incident angle
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

////////////////////////////////////////////////////////////////////////////////////
Sphere SphereConstructor(vec3 center, float radius, int materialType, int material)
{
	Sphere sphere;

	sphere.center = center;
	sphere.radius = radius;
	sphere.materialType = materialType;
	sphere.material = material;

	return sphere;
}

////////////////////////////////////////////////////////////////////////////////////
bool SphereHit(Sphere sphere, Ray ray, float t_min, float t_max, inout HitRecord hitRecord)
{
	vec3 oc = ray.origin - sphere.center;
	
	float a = dot(ray.direction, ray.direction);
	float b = dot(oc, ray.direction);
	float c = dot(oc, oc) - sphere.radius * sphere.radius;

	float discriminant = b * b - a * c;
	if(discriminant>0)
	{
		float temp = (-b - sqrt(discriminant)) / a;
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
World WorldConstructor()
{
	World world;

	world.objectCount = 4;
	world.objects[0] = SphereConstructor(vec3( 0.0,    0.0, -1.0), 0.25, MAT_LAMBERTIAN, 0);
	world.objects[1] = SphereConstructor(vec3( 0.7,    0.0, -1.0), 0.25, MAT_METALLIC, 1);
	world.objects[2] = SphereConstructor(vec3(-0.7,    0.0, -1.0), 0.25, MAT_DIELECTRIC, 2);
	world.objects[3] = SphereConstructor(vec3( 0.0, -100.5, -1.0), 100.0, MAT_LAMBERTIAN, 3);

	return world;
}

bool WorldHit(World world, Ray ray, float t_min, float t_max, inout HitRecord rec)
{
	float cloestSoFar = t_max;
	bool hitSomething = false;
	HitRecord tempRec;

	for(int i=0; i<world.objectCount; i++)
	{
		if(SphereHit(world.objects[i], ray, t_min, cloestSoFar, tempRec))
		{
			hitSomething = true;
			cloestSoFar = tempRec.t;
			
			rec = tempRec;
		}
	}

	return hitSomething;
}

/////////////////////////////////////////////////////////////////////////////////
World world;
Camera camera;
Lambertian lambertMaterials[4];
Metallic metallicMaterials[4];
Dielectric dielectricMaterials[4];

void InitScene()
{
	world = WorldConstructor();
	camera = CameraConstructor(vec3(-2.0, -1.0, -1.0), vec3(4.0, 0.0, 0.0), vec3(0.0, 2.0, 0.0), vec3(0.0, 0.0, 0.0));
	camera = CameraSet(cameraPos, cameraTarget, cameraUp, 90.0, screenSize.x / screenSize.y);

	lambertMaterials[0] = LambertianConstructor(vec3(0.7, 0.5, 0.5));
	lambertMaterials[1] = LambertianConstructor(vec3(0.5, 0.7, 0.5));
	lambertMaterials[2] = LambertianConstructor(vec3(0.5, 0.5, 0.7));
	lambertMaterials[3] = LambertianConstructor(vec3(0.7, 0.7, 0.7));

	metallicMaterials[0] = MetallicConstructor(vec3(0.7, 0.5, 0.5), 0.0);
	metallicMaterials[1] = MetallicConstructor(vec3(0.5, 0.7, 0.5), 0.1);
	metallicMaterials[2] = MetallicConstructor(vec3(0.5, 0.5, 0.7), 0.2);
	metallicMaterials[3] = MetallicConstructor(vec3(0.7, 0.7, 0.7), 0.3);

	dielectricMaterials[0] = DielectricConstructor(vec3(1.0, 1.0, 1.0), 0.0, 1.5);
	dielectricMaterials[1] = DielectricConstructor(vec3(1.0, 1.0, 1.0), 0.1, 1.5);
	dielectricMaterials[2] = DielectricConstructor(vec3(1.0, 1.0, 1.0), 0.2, 1.5);
	dielectricMaterials[3] = DielectricConstructor(vec3(1.0, 1.0, 1.0), 0.3, 1.5);
}

bool MaterialScatter(in int materialType, in int material, in Ray incident, in HitRecord hitRecord, out Ray scatter, out vec3 attenuation)
{
	if(materialType==MAT_LAMBERTIAN)
		return LambertianScatter(lambertMaterials[material], incident, hitRecord, scatter, attenuation);
	else if(materialType==MAT_METALLIC)
		return MetallicScatter(metallicMaterials[material], incident, hitRecord, scatter, attenuation);
	else if(materialType==MAT_DIELECTRIC)
		return DielectricScatter(dielectricMaterials[material], incident, hitRecord, scatter, attenuation);
	else
		return false;
}

vec3 GetEnvironmentColor(World world, Ray ray)
{
	//vec3 unit_direction = normalize(ray.direction);
	//float t = 0.5 * (unit_direction.y + 1.0);
	//return vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;

	vec3 dir = normalize(ray.direction);
	float phi = acos(dir.y) / PI;
	float theta = (atan(dir.x, dir.z) + (PI / 2.0)) / PI;
	return texture(envMap, vec2(theta, phi)).xyz;
}

/*
vec3 WorldTrace(World world, Ray ray)
{
	HitRecord hitRecord;
	if(WorldHit(world, ray, 0.001, RAYCAST_MAX, hitRecord))
	{
		ray = RayConstructor(hitRecord.position, hitRecord.normal + random_in_unit_sphere());
		WorldTrace(world, ray);
	}
	else
	{
		return GetEnvironmentColor(world, ray);
	}
}
*/

vec3 WorldTrace(World world, Ray ray, int depth)
{
	HitRecord hitRecord;

	vec3 frac = vec3(1.0, 1.0, 1.0);
	vec3 bgColor = vec3(0.0, 0.0, 0.0);
	while(depth>0)
	{
		depth--;
		if(WorldHit(world, ray, 0.001, RAYCAST_MAX, hitRecord))
		{
			Ray scatterRay;
			vec3 attenuation;
			if(!MaterialScatter(hitRecord.materialType, hitRecord.material, ray, hitRecord, scatterRay, attenuation))
				break;
			
			frac *= attenuation;
			ray = scatterRay;
		}
		else
		{
			bgColor = GetEnvironmentColor(world, ray);
			break;
		}
	}

	return bgColor * frac;
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
	InitScene();
	
	vec3 col = vec3(0.0, 0.0, 0.0);
	int ns = 100;
	for(int i=0; i<ns; i++)
	{
		Ray ray = CameraGetRay(camera, screenCoord + rand2() / screenSize);
		col += WorldTrace(world, ray, 50);
	}
	col /= ns;

	//col = GammaCorrection(col);

	FragColor.xyz = col;
	FragColor.w = 1.0;
}