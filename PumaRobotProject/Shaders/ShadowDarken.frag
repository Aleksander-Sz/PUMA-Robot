#version 330 core
out vec4 FragColor;
uniform vec3 darken;
void main()
{
	FragColor = vec4(darken, 1.0);
}
