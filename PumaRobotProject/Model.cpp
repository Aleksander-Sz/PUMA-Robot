#include "Model.h"

void Model::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glBindVertexArray(0);
}

void Model::Draw(Shader& shader)
{
	shader.setMat4("model", model);
	//draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Model::loadModel(std::string path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cout << "Failed to open the file\n";
		return;
	}

	int vertexPositionCount;
	file >> vertexPositionCount;  // read integer directly

	std::cout << vertexPositionCount << ".\n";
	std::vector<glm::vec3> vertexPositions;

	for (int i = 0; i < vertexPositionCount; i++)
	{
		float x, y, z;
		file >> x >> y >> z;  // read 3 floats per line
		vertexPositions.push_back(glm::vec3(x, y, z));
	}

	int vertexCount;
	file >> vertexCount;

	for (int i = 0; i < vertexCount; i++)
	{
		int vertexId;
		file >> vertexId;
		Vertex newVertex;
		newVertex.position = vertexPositions[vertexId];

		float x, y, z;
		file >> x >> y >> z;  // read 3 floats per line
		newVertex.normal = glm::vec3(x, y, z);
		vertices.push_back(newVertex);
	}

	int triangleCount;
	file >> triangleCount;

	for (int i = 0; i < triangleCount; i++)
	{
		int a, b, c;
		file >> a >> b >> c;
		indices.push_back(a);
		indices.push_back(b);
		indices.push_back(c);
	}

	file.close();
	setupMesh();
}

Model::Model()
{
	;
}
Model::Model(std::string path)
{
	loadModel(path);
}

void Model::Plane(float size, glm::mat4 position)
{
	model = position;
	size /= 2.0f;
	Vertex vertex;
	vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	vertex.position = glm::vec3(-size, 0.0f, -size);
	vertices.push_back(vertex);
	vertex.position = glm::vec3(size, 0.0f, -size);
	vertices.push_back(vertex);
	vertex.position = glm::vec3(-size, 0.0f, size);
	vertices.push_back(vertex);
	vertex.position = glm::vec3(size, 0.0f, size);
	vertices.push_back(vertex);

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);

	setupMesh();
}