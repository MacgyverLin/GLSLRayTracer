#version 330 core
#define PI 3.14159265
#define RAYCAST_MAX 100000.0
in vec2 screenCoord;

uniform vec2 screenSize;
uniform int sampleCount;

uniform samplerCube envMap;
uniform float envMapIntensity;
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

	float aperture;
	float focalDistance;
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

	vec3 lower_left_corner = camera.origin - halfWidth * camera.focalDistance * u - halfHeight * camera.focalDistance * v - camera.focalDistance * w;
	vec3 horizontal = 2.0 * halfWidth * camera.focalDistance * u;
	vec3 vertical = 2.0 * halfHeight * camera.focalDistance * v;

	Ray ray;
	vec3 lenoffset = random_in_unit_disk() * (camera.aperture / 2.0);
	ray.origin = camera.origin + lenoffset;
	ray.direction = lower_left_corner + offset.x * horizontal + offset.y * vertical - (camera.origin + lenoffset);
	return ray;
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

bool LambertianScatter(in Lambertian lambertian, in Ray incident, in HitRecord hitRecord, out Ray scattered, out vec3 attenuation)
{
	attenuation = lambertian.albedo;

	scattered.origin = hitRecord.position;
	scattered.direction = hitRecord.normal + random_in_unit_sphere() ;

	return true;
}

struct Metallic
{
	vec3 albedo;
	float roughness;
};

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
	scattered.direction = reflect(incident.direction, hitRecord.normal) + random_in_unit_sphere() * metallic.roughness;

	return dot(scattered.direction, hitRecord.normal) > 0.0;
}

struct Dielectric
{
	vec3 albedo;
	float roughness;
	float ior;
};

bool DielectricScatter(in Dielectric dielectric, in Ray incident, in HitRecord hitRecord, out Ray scattered, out vec3 attenuation)
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

struct PhysicallyBase
{
	vec3 albedo;
	vec3 specColor;
	float metallic;
	float roughness;
	float anisotropic;
};

float BlinnPhongNormalDistribution(float NdotH, float specularpower, float speculargloss)
{
    float Distribution = pow(NdotH,speculargloss) * specularpower;
    Distribution *= (2+specularpower) / (2*3.1415926535);
    return Distribution;
}

float PhongNormalDistribution(float RdotV, float specularpower, float speculargloss){
    float Distribution = pow(RdotV,speculargloss) * specularpower;
    Distribution *= (2+specularpower) / (2*3.1415926535);
    return Distribution;
}

float BeckmannNormalDistribution(float roughness, float NdotH)
{
    float roughnessSqr = roughness*roughness;
    float NdotHSqr = NdotH*NdotH;
    return max(0.000001,(1.0 / (3.1415926535*roughnessSqr*NdotHSqr*NdotHSqr)) * exp((NdotHSqr-1)/(roughnessSqr*NdotHSqr)));
}

float GaussianNormalDistribution(float roughness, float NdotH)
{
    float roughnessSqr = roughness*roughness;
	float thetaH = acos(NdotH);
    return exp(-thetaH*thetaH/roughnessSqr);
}

float sqr(float v)
{
	return v * v;
}

float GGXNormalDistribution(float roughness, float NdotH)
{
    float roughnessSqr = roughness*roughness;
    float NdotHSqr = NdotH*NdotH;
    float TanNdotHSqr = (1-NdotHSqr)/NdotHSqr;

    return (1.0/3.1415926535) * sqr(roughness/(NdotHSqr * (roughnessSqr + TanNdotHSqr)));
}

float TrowbridgeReitzNormalDistribution(float NdotH, float roughness){
    float roughnessSqr = roughness*roughness;
    float Distribution = NdotH*NdotH * (roughnessSqr-1.0) + 1.0;
    return roughnessSqr / (3.1415926535 * Distribution*Distribution);
}

float TrowbridgeReitzAnisotropicNormalDistribution(float anisotropic, float glossiness, float NdotH, float HdotX, float HdotY)
{
    float aspect = sqrt(1.0-anisotropic * 0.9);
    float X = max(.001, sqr(1.0-glossiness)/aspect) * 5;
    float Y = max(.001, sqr(1.0-glossiness)*aspect) * 5;
    
    return 1.0 / (3.1415926535 * X*Y * sqr(sqr(HdotX/X) + sqr(HdotY/Y) + NdotH*NdotH));
}

float WardAnisotropicNormalDistribution(float anisotropic, float glossiness, float NdotL,
 float NdotV, float NdotH, float HdotX, float HdotY){
    float aspect = sqrt(1.0-anisotropic * 0.9);
    float X = max(.001, sqr(1.0-glossiness)/aspect) * 5;
 	float Y = max(.001, sqr(1.0-glossiness)*aspect) * 5;
    float exponent = -(sqr(HdotX/X) + sqr(HdotY/Y)) / sqr(NdotH);
    float Distribution = 1.0 / (4.0 * 3.14159265 * X * Y * sqrt(NdotL * NdotV));
    Distribution *= exp(exponent);
    return Distribution;
}

float ImplicitGeometricShadowingFunction (float NdotL, float NdotV){
	float Gs = (NdotL*NdotV);       
	return Gs;
}

float AshikhminShirleyGSF (float NdotL, float NdotV, float LdotH){
	float Gs = NdotL*NdotV/(LdotH*max(NdotL,NdotV));
	return  (Gs);
}

float AshikhminPremozeGeometricShadowingFunction (float NdotL, float NdotV){
	float Gs = NdotL*NdotV/(NdotL+NdotV - NdotL*NdotV);
	return  (Gs);
}

float DuerGeometricShadowingFunction (vec3 lightDirection,vec3 viewDirection, vec3 normalDirection,float NdotL, float NdotV)
{
    vec3 LpV = lightDirection + viewDirection;
    float Gs = dot(LpV,LpV) * pow(dot(LpV, normalDirection),-4.0);
    return  (Gs);
}

float NeumannGeometricShadowingFunction (float NdotL, float NdotV)
{
	float Gs = (NdotL*NdotV)/max(NdotL, NdotV);       
	return  (Gs);
}

float KelemenGeometricShadowingFunction (float NdotL, float NdotV, float LdotV, float VdotH){
	float Gs = (NdotL*NdotV)/(VdotH * VdotH); 
	return   (Gs);
}

float KurtGeometricShadowingFunction (float NdotL, float NdotV, float VdotH, float roughness)
{
	float Gs =  NdotL*NdotV/(VdotH*pow(NdotL*NdotV, roughness));
	return  (Gs);
}

float CookTorrenceGeometricShadowingFunction (float NdotL, float NdotV, float VdotH, float NdotH)
{
	float Gs = min(1.0, min(2*NdotH*NdotV / VdotH, 2*NdotH*NdotL / VdotH));
	return  (Gs);
}

float SchlickGeometricShadowingFunction (float NdotL, float NdotV, float roughness)
{
    float roughnessSqr = roughness*roughness;

	float SmithL = (NdotL)/(NdotL * (1-roughnessSqr) + roughnessSqr);
	float SmithV = (NdotV)/(NdotV * (1-roughnessSqr) + roughnessSqr);

	return (SmithL * SmithV); 
}

float SchlickGGXGeometricShadowingFunction (float NdotL, float NdotV, float roughness)
{
    float k = roughness / 2;

    float SmithL = (NdotL)/ (NdotL * (1- k) + k);
    float SmithV = (NdotV)/ (NdotV * (1- k) + k);

	float Gs =  (SmithL * SmithV);
	return Gs;
}

float GGXGeometricShadowingFunction (float NdotL, float NdotV, float roughness)
{
    float roughnessSqr = roughness*roughness;
    float NdotLSqr = NdotL*NdotL;
    float NdotVSqr = NdotV*NdotV;

    float SmithL = (2 * NdotL)/ (NdotL + sqrt(roughnessSqr + ( 1-roughnessSqr) * NdotLSqr));
    float SmithV = (2 * NdotV)/ (NdotV + sqrt(roughnessSqr + ( 1-roughnessSqr) * NdotVSqr));

	float Gs =  (SmithL * SmithV);
	return Gs;
}

float SchlickFresnel(float i)
{
    float x = clamp(1.0-i, 0.0, 1.0);
    float x2 = x*x;
    return x2*x2*x;
}

vec3 SchlickFresnelFunction(vec3 SpecularColor, float LdotH)
{
    return SpecularColor + (vec3(1.0, 1.0, 1.0) - SpecularColor) * SchlickFresnel(LdotH); // approach to 1 quickly near 90 degree
}

float SchlickIORFresnelFunction(float ior, float LdotH)
{
    float f0 = pow((ior-1) / (ior+1), 2);
    return f0 + (1 - f0) * SchlickFresnel(LdotH);
}

float SphericalGaussianFresnelFunction(float LdotH,float SpecularColor)
{	
	float power = ((-5.55473 * LdotH) - 6.98316) * LdotH;
    return SpecularColor + (1 - SpecularColor)  * pow(2, power);
}

bool PhysicallyBaseDiffuseScatter(in PhysicallyBase material, in Ray incident, in HitRecord hitRecord, out Ray scattered, out vec3 attenuation)
{
	//scattered.direction = reflect(incident.direction, hitRecord.normal);
	//attenuation = vec3(1.0, 0.0, 0.0);
	//return true;

	attenuation = material.albedo / PI;

	scattered.origin = hitRecord.position;
	scattered.direction = hitRecord.normal + random_in_unit_sphere();

	return true;
}

bool PhysicallyBaseSpecularScatter(in PhysicallyBase material, in Ray incident, in HitRecord hitRecord, out Ray scattered, out vec3 attenuation)
{
	vec3 scatterDirection = reflect(incident.direction, hitRecord.normal);

	// prepare all variable
	vec3 normalDirection = normalize(hitRecord.normal);

	vec3 viewDirection = -normalize(incident.direction);
	
	vec3 lightDirection = vec3(0.0, 0.0, 1.0);

	vec3 halfDirection = normalize(viewDirection + lightDirection);
	
	float NdotL = max(0.0, dot(normalDirection, lightDirection));

	float NdotH = max(0.0, dot(normalDirection, halfDirection));

	float NdotV = max(0.0, dot(normalDirection, viewDirection));

	float VdotH = max(0.0, dot(viewDirection, halfDirection));

	float LdotH = max(0.0, dot(lightDirection, halfDirection));
 
	float LdotV = max(0.0, dot(lightDirection, viewDirection)); 

	material.roughness = sqr(material.roughness);
	float glossiness = sqr(1.0 - material.roughness);
	float roughness = material.roughness + 0.001;
	float anisotropic = material.anisotropic;
	vec3 tangent = vec3(1.0, 0.0, 0.0);
	vec3 bitangent = cross(normalDirection, tangent);
	vec3 color = mix(material.albedo, material.specColor, material.metallic);

	vec3 SpecularDistribution = vec3(1.0, 1.0, 1.0);
	//SpecularDistribution *= BlinnPhongNormalDistribution(NdotH, glossiness, max(1, glossiness * 40));
	//SpecularDistribution *= BeckmannNormalDistribution(roughness, NdotH);
	//SpecularDistribution *= GaussianNormalDistribution(roughness, NdotH);
	SpecularDistribution *= GGXNormalDistribution(roughness, NdotH);
	//SpecularDistribution *= TrowbridgeReitzNormalDistribution(NdotH, roughness);
	//SpecularDistribution *= TrowbridgeReitzAnisotropicNormalDistribution(anisotropic, glossiness, NdotH, dot(halfDirection, tangent),  dot(halfDirection, bitangent));
	//SpecularDistribution *= WardAnisotropicNormalDistribution(anisotropic, glossiness, NdotL, NdotV, NdotH, dot(halfDirection, tangent), dot(halfDirection, bitangent));

	float GeometricShadow = 1;
	// GeometricShadow *= ImplicitGeometricShadowingFunction(NdotL, NdotV);
	// GeometricShadow *= AshikhminShirleyGSF(NdotL, NdotV, LdotH);
	// GeometricShadow *= AshikhminPremozeGeometricShadowingFunction(NdotL, NdotV);
	// GeometricShadow *= DuerGeometricShadowingFunction(lightDirection, viewDirection, normalDirection, NdotL, NdotV);
	// GeometricShadow *= NeumannGeometricShadowingFunction(NdotL, NdotV);
	// GeometricShadow *= KelemenGeometricShadowingFunction(NdotL, NdotV, LdotV,  VdotH);
	// GeometricShadow *= KurtGeometricShadowingFunction(NdotL, NdotV, VdotH, roughness);
	// GeometricShadow *= CookTorrenceGeometricShadowingFunction(NdotL, NdotV, VdotH, NdotH);
	// GeometricShadow *= SchlickGeometricShadowingFunction(NdotL, NdotV, roughness);
	GeometricShadow *= SchlickGGXGeometricShadowingFunction(NdotL, NdotV, roughness);
	// GeometricShadow *= GGXGeometricShadowingFunction(NdotL, NdotV, roughness);

	vec3 FresnelFunction = vec3(1.0, 1.0, 1.0);
	FresnelFunction *= SchlickFresnelFunction(color, LdotH);
	//FresnelFunction *= SchlickIORFresnelFunction(1.5, LdotH);

	scattered.origin = hitRecord.position;
	scattered.direction = scatterDirection;
	attenuation = color;
	attenuation *= SpecularDistribution;
	attenuation *= vec3(GeometricShadow);
	//attenuation *= FresnelFunction;

	return true;
}

bool PhysicallyBaseScatter(in PhysicallyBase material, in Ray incident, in HitRecord hitRecord, out Ray scattered, out vec3 attenuation)
{
	if(rand() > material.metallic)
	{
		return PhysicallyBaseDiffuseScatter(material, incident, hitRecord, scattered, attenuation);
	}
	else
	{
		return PhysicallyBaseSpecularScatter(material, incident, hitRecord, scattered, attenuation);
	}
}

/////////////////////////////////////////////////////////////////////////////////
uniform World world;
uniform Camera camera;
uniform Lambertian lambertMaterials[4];
uniform Metallic metallicMaterials[4];
uniform Dielectric dielectricMaterials[4];
uniform PhysicallyBase pbrMaterials[10];

bool MaterialScatter(in Ray incident, in HitRecord hitRecord, out Ray scatter, out vec3 attenuation)
{
	if(hitRecord.materialType==MAT_LAMBERTIAN)
		return LambertianScatter(lambertMaterials[hitRecord.material], incident, hitRecord, scatter, attenuation);
	else if(hitRecord.materialType==MAT_METALLIC)
		return MetallicScatter(metallicMaterials[hitRecord.material], incident, hitRecord, scatter, attenuation);
	else if(hitRecord.materialType==MAT_DIELECTRIC)
		return DielectricScatter(dielectricMaterials[hitRecord.material], incident, hitRecord, scatter, attenuation);
	else if(hitRecord.materialType==MAT_PBR)
		return PhysicallyBaseScatter(pbrMaterials[hitRecord.material], incident, hitRecord, scatter, attenuation);
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
	//return vec3(0.7, 0.7, 0.7);
	//vec3 unit_direction = normalize(ray.direction);
	//float t = 0.5 * (unit_direction.y + 1.0);
	//return vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;
	
	//vec3 dir = normalize(ray.direction);
	//float theta = acos(dir.y) / PI;
	//float phi = (atan(dir.z, dir.x) + (PI / 2.0)) / PI;
	//return texture(envMap, vec3(phi, theta, 0.0)).xyz;

	vec3 dir = normalize(ray.direction);
	return texture(envMap, dir).xyz * envMapIntensity;
}

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
	return pow(c, vec3(1 / 2.2));
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

	////FragColor.xyz = col;
	FragColor.xyz = GammaCorrection(col);
	FragColor.w = 1.0;
}