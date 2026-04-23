#version 330 core

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
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

uniform vec3 viewPos;
uniform int numLights;
#define NR_LIGHTS 10
uniform Light lights[NR_LIGHTS];

uniform Material material;

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
	//texture
	vec3 diffuseColor = vec3(1.0, 1.0, 1.0);
	vec3 specularColor = vec3(1.0, 1.0, 1.0);
	// ambient
	vec3 ambient = diffuseColor * lights[0].ambient;
	
	vec3 normalizedNormal = normalize(normal);
	vec3 totalColor = ambient;
	for(int i = 0; i < numLights; i++)
	{
		vec4 directionAndAttenuation;
		if(lights[i].type == 0)
		{
			directionAndAttenuation = CalcDirectionalLight(lights[i]);
		}
		else if(lights[i].type == 1)
		{
			directionAndAttenuation = CalcPointLight(lights[i], fragPos);
		}
		else if(lights[i].type == 2)
		{
			directionAndAttenuation = CalcSpotLight(lights[i], fragPos);
		}
		// diffuse
		float diff = max(dot(normalizedNormal, directionAndAttenuation.xyz), 0.0f);
		vec3 diffuse = lights[i].diffuse * diff * diffuseColor;
		// specular
		vec3 viewDir = normalize(viewPos - fragPos);
		vec3 reflectDir = reflect(-directionAndAttenuation.xyz, normalizedNormal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
		vec3 specular = spec * lights[i].specular * specularColor;
		vec3 finalColor = (diffuse + specular) * directionAndAttenuation.w;
		totalColor += finalColor;
	}
	FragColor = vec4(totalColor, 1.0f);
}