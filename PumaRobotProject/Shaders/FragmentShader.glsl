#version 330 core

struct Material {
	vec3 specular;
	float shininess;
};
struct Light {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;
	int type;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

out vec4 FragColor;

in vec4 vertexColor;
in vec2 texCoords;
in vec3 normal;
in vec3 fragPos;
flat in int surfaceType;

uniform vec3 viewPos;
uniform Light light;

uniform Material material;
uniform vec3 color;
uniform float alpha;
uniform bool useMetalSheetTex;
uniform sampler2D metalSheetTex;
uniform vec3 mirrorOrigin;
uniform vec3 mirrorUAxis;
uniform vec3 mirrorVAxis;
uniform float mirrorSize;
uniform bool useRoomTex;
uniform sampler2D wallTex;
uniform sampler2D floorTex;
uniform float wallTexScale;
uniform float floorTexScale;
uniform float ceilingTexScale;
uniform float floorUvRotation;
uniform float floorUvShear;

vec4 CalcPointLight(Light inLight, vec3 inFragPos)
{
	vec4 directionAndAttenuation;
	directionAndAttenuation.xyz = normalize(inLight.position - inFragPos);
	float distance = length(inLight.position - inFragPos);
	directionAndAttenuation.w = 1.0 / (inLight.constant + inLight.linear * distance + inLight.quadratic * (distance * distance));
	return directionAndAttenuation;
}
vec4 CalcDirectionalLight(Light inLight)
{
	return vec4(-normalize(inLight.direction), 1.0f);
}
vec4 CalcSpotLight(Light inLight, vec3 inFragPos)
{
	vec4 directionAndAttenuation;
	vec3 rayDirection = normalize(inLight.position - inFragPos);
	directionAndAttenuation.xyz = rayDirection;
	float theta = dot(rayDirection, -normalize(inLight.direction));
	float epsilon = inLight.cutOff - inLight.outerCutOff;
	float distance = length(inLight.position - inFragPos);
	float intensity = clamp((theta - inLight.outerCutOff) / epsilon, 0.0, 1.0);
	directionAndAttenuation.w = intensity * 1.0 / (inLight.constant + inLight.linear * distance + inLight.quadratic * (distance * distance));
	return directionAndAttenuation;
}

void main()
{
	vec3 N = normalize(normal);
	vec3 albedo = color;
	vec3 roomSpecMap = vec3(1.0);
	if (useMetalSheetTex)
	{
		vec3 rel = fragPos - mirrorOrigin;
		float u = dot(rel, mirrorUAxis) / mirrorSize + 0.5;
		float v = dot(rel, mirrorVAxis) / mirrorSize + 0.5;
		vec3 texAlbedo = texture(metalSheetTex, vec2(u, v)).rgb;
		albedo *= texAlbedo;
	}
	if (useRoomTex)
	{
		vec2 uv;
		if (surfaceType == 1)
		{
			uv = texCoords * ceilingTexScale;
		}
		else if (surfaceType == 2)
		{
			uv = texCoords * ceilingTexScale;
		}
		else
		{
			uv = texCoords * wallTexScale;
		}
		albedo *= texture(wallTex, uv).rgb;
		roomSpecMap = texture(floorTex, uv).rgb;
	}
	vec3 diffuseColor = albedo;
	vec3 specularCoeff = material.specular * roomSpecMap;
	vec3 ambient = diffuseColor * light.ambient;

	vec3 totalColor = ambient;

	vec4 directionAndAttenuation;
	if (light.type == 0)
		directionAndAttenuation = CalcDirectionalLight(light);
	else if (light.type == 1)
		directionAndAttenuation = CalcPointLight(light, fragPos);
	else if (light.type == 2)
		directionAndAttenuation = CalcSpotLight(light, fragPos);

	vec3 L = directionAndAttenuation.xyz;
	float diff = max(dot(N, L), 0.0);
	vec3 diffuse = light.diffuse * diff * diffuseColor;

	vec3 V = normalize(viewPos - fragPos);
	vec3 H = normalize(L + V);
	float spec = pow(max(dot(N, H), 0.0), material.shininess);
	vec3 specular = spec * light.specular * specularCoeff;

	vec3 lit = (diffuse + specular) * directionAndAttenuation.w;
	totalColor += lit;

	FragColor = vec4(totalColor, alpha);
}