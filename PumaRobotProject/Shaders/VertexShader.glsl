#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 textureCoordinates;
layout (location = 3) in float vertexSurfaceType;

out vec4 vertexColor;
out vec2 texCoords;
out vec3 normal;
out vec3 fragPos;
flat out int surfaceType;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 clipPlane;
uniform bool useClipPlane;

void main()
{
	vec4 worldPos = model * vec4(aPos, 1.0);
	gl_Position = projection * view * worldPos;
	vertexColor = vec4((aPos.x+1)/2, (aPos.y+1)/2, 0.2f, 1.0f);
	texCoords = textureCoordinates;
	normal = mat3(transpose(inverse(model))) * vertexNormal;
	fragPos = vec3(worldPos);
	surfaceType = int(vertexSurfaceType + 0.5);
	gl_ClipDistance[0] = useClipPlane ? dot(worldPos, clipPlane) : 1.0;
}