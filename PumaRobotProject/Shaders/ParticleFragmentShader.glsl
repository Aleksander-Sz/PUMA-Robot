#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;

uniform float sparkAlpha;

void main()
{
	float a = sparkAlpha * 0.55;
	FragColor = vec4(1.0, 0.35, 0.08, a);
}
