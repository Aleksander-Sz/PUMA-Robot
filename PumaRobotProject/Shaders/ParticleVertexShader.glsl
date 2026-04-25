#version 330 core

layout (location = 0) in int id;

out vec3 normal;
out vec3 fragPos;

uniform vec3 top;
uniform vec3 bottom;
uniform mat4 viewProjection;

void main()
{
	vec3 worldPos = bottom;
	if(id == 0)
		worldPos = top;
	gl_Position = viewProjection * vec4(worldPos, 1.0);
	fragPos = vec3(worldPos);
}