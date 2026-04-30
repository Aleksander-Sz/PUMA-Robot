#ifndef MODEL_H
#define MODEL_H

#include <glad/gl.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <iostream>
#include <fstream>

struct MeshEdge
{
	unsigned p0, p1, t0, t1;
};

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	float surfaceType;
};

class Model
{
public:
	Model();
	Model(std::string path);
	void Draw(Shader& shader);
	void Plane(float size, glm::mat4 position);
	void Cylinder(float radius, float lenght, glm::mat4 position);
	void Room(float size, glm::mat4 position);
	glm::mat4 model = glm::mat4(1.0f);

	/// Shadow volume: world matrix, light in world space. Appends vec4(xyz, w): w=1 model pos,
	/// w=0 homogeneous direction (P - light) for infinitely extruded verts (back cap + sides).
	void AppendShadowVolumes(const glm::mat4& worldModel, const glm::vec3& lightWorld,
		std::vector<glm::vec4>& triangleVerts) const;

private:
	// model data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> basePositions;
	std::vector<glm::uvec3> meshTriangles;
	std::vector<MeshEdge> meshEdges;
	bool hasSilhouetteData = false;

	void setupMesh();
	void loadModel(std::string path);
	void clearSilhouetteData();
	unsigned int VAO, VBO, EBO;
};
#endif
