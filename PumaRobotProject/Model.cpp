#include "Model.h"
#include <cmath>
#include <cstddef>
#include <cstdint>

void Model::clearSilhouetteData()
{
	basePositions.clear();
	meshTriangles.clear();
	meshEdges.clear();
	hasSilhouetteData = false;
}

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

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, surfaceType));

	glBindVertexArray(0);
}

void Model::Draw(Shader& shader)
{
	shader.use();
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
	basePositions = vertexPositions;

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
		newVertex.texCoords = glm::vec2(0.0f, 0.0f);
		newVertex.surfaceType = 0.0f;
		vertices.push_back(newVertex);
	}

	int triangleCount;
	file >> triangleCount;
	meshTriangles.reserve((size_t)triangleCount);

	for (int i = 0; i < triangleCount; i++)
	{
		int a, b, c;
		file >> a >> b >> c;
		indices.push_back(a);
		indices.push_back(b);
		indices.push_back(c);
		meshTriangles.push_back(glm::uvec3((unsigned)a, (unsigned)b, (unsigned)c));
	}

	int edgeCount = 0;
	file >> edgeCount;
	meshEdges.reserve((size_t)edgeCount);
	for (int i = 0; i < edgeCount; i++)
	{
		unsigned p0, p1, t0, t1;
		file >> p0 >> p1 >> t0 >> t1;
		meshEdges.push_back(MeshEdge{ p0, p1, t0, t1 });
	}
	hasSilhouetteData = edgeCount > 0 && !meshTriangles.empty() && !basePositions.empty();

	file.close();
	setupMesh();
}

Model::Model()
{
	clearSilhouetteData();
}
Model::Model(std::string path)
{
	vertices.clear();
	indices.clear();
	clearSilhouetteData();
	loadModel(path);
}

void Model::Plane(float size, glm::mat4 position)
{
	clearSilhouetteData();
	vertices.clear();
	indices.clear();

	model = position;
	size /= 2.0f;
	Vertex vertex;
	vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	vertex.position = glm::vec3(-size, 0.0f, -size);
	vertex.texCoords = glm::vec2(0.0f, 0.0f);
	vertex.surfaceType = 0.0f;
	vertices.push_back(vertex);
	vertex.position = glm::vec3(size, 0.0f, -size);
	vertex.texCoords = glm::vec2(1.0f, 0.0f);
	vertices.push_back(vertex);
	vertex.position = glm::vec3(-size, 0.0f, size);
	vertex.texCoords = glm::vec2(0.0f, 1.0f);
	vertices.push_back(vertex);
	vertex.position = glm::vec3(size, 0.0f, size);
	vertex.texCoords = glm::vec2(1.0f, 1.0f);
	vertices.push_back(vertex);

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(3);
	indices.push_back(1);
	indices.push_back(2);

	setupMesh();
}

void Model::Cylinder(float radius, float length, glm::mat4 position)
{
	clearSilhouetteData();
	vertices.clear();
	indices.clear();

	model = position;
	for (size_t i = 0; i < 24; i++)
	{
		float x = radius * std::sin(2.0f * 3.14159f * i / 24.0f);
		float z = radius * std::cos(2.0f * 3.14159f * i / 24.0f);
		Vertex newVertex;
		newVertex.position = glm::vec3(x, -length / 2.0f, z);
		newVertex.normal = glm::normalize(glm::vec3(x, 0.0f, z));
		newVertex.texCoords = glm::vec2((float)i / 24.0f, 0.0f);
		newVertex.surfaceType = 0.0f;
		vertices.push_back(newVertex);
		newVertex.position.y = length / 2.0f;
		newVertex.texCoords = glm::vec2((float)i / 24.0f, 1.0f);
		vertices.push_back(newVertex);
		indices.push_back(i * 2);
		indices.push_back((i + 1) % 24 * 2);
		indices.push_back(i * 2 + 1);
		indices.push_back(i * 2 + 1);
		indices.push_back((i + 1) % 24 * 2);
		indices.push_back((i + 1) % 24 * 2 + 1);
	}
	Vertex bottomCenter;
	bottomCenter.position = glm::vec3(0.0f, -length / 2.0f, 0.0f);
	bottomCenter.normal = glm::vec3(0.0f, -1.0f, 0.0f);
	bottomCenter.texCoords = glm::vec2(0.5f, 0.5f);
	bottomCenter.surfaceType = 0.0f;
	Vertex topCenter;
	topCenter.position = glm::vec3(0.0f, length / 2.0f, 0.0f);
	topCenter.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	topCenter.texCoords = glm::vec2(0.5f, 0.5f);
	topCenter.surfaceType = 0.0f;
	size_t baseIndex = vertices.size();

	// centers
	vertices.push_back(bottomCenter); // baseIndex
	vertices.push_back(topCenter);    // baseIndex + 1

	// ring
	for (size_t i = 0; i < 24; i++)
	{
		float x = radius * std::sin(2.0f * 3.14159f * i / 24.0f);
		float z = radius * std::cos(2.0f * 3.14159f * i / 24.0f);

		Vertex v;

		v.position = glm::vec3(x, -length / 2.0f, z);
		v.normal = glm::vec3(0, -1, 0);
		v.texCoords = glm::vec2(0.5f + x / (2.0f * radius), 0.5f + z / (2.0f * radius));
		v.surfaceType = 0.0f;
		vertices.push_back(v); // baseIndex + 2 + i*2

		v.position = glm::vec3(x, length / 2.0f, z);
		v.normal = glm::vec3(0, 1, 0);
		v.texCoords = glm::vec2(0.5f + x / (2.0f * radius), 0.5f + z / (2.0f * radius));
		v.surfaceType = 0.0f;
		vertices.push_back(v); // baseIndex + 3 + i*2
	}
	for (size_t i = 0; i < 24; i++)
	{
		size_t curr = baseIndex + 2 + i * 2;
		size_t next = baseIndex + 2 + ((i + 1) % 24) * 2;

		// bottom cap (flip winding if needed)
		indices.push_back(baseIndex);
		indices.push_back(next);
		indices.push_back(curr);

		// top cap
		indices.push_back(baseIndex + 1);
		indices.push_back(curr + 1);
		indices.push_back(next + 1);
	}
	setupMesh();
}

void Model::Room(float size, glm::mat4 position)
{
	clearSilhouetteData();
	model = position;
	size /= 2.0f;

	vertices.clear();
	indices.clear();

	auto pushQuad = [&](glm::vec3 n, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float surfaceType)
	{
		unsigned base = (unsigned)vertices.size();
		Vertex v;
		v.normal = n;
		v.surfaceType = surfaceType;
		v.position = p0; v.texCoords = glm::vec2(0.0f, 0.0f); vertices.push_back(v);
		v.position = p1; v.texCoords = glm::vec2(1.0f, 0.0f); vertices.push_back(v);
		v.position = p2; v.texCoords = glm::vec2(1.0f, 1.0f); vertices.push_back(v);
		v.position = p3; v.texCoords = glm::vec2(0.0f, 1.0f); vertices.push_back(v);
		indices.push_back(base + 0); indices.push_back(base + 1); indices.push_back(base + 2);
		indices.push_back(base + 0); indices.push_back(base + 2); indices.push_back(base + 3);
	};

	// FRONT (+Z), BACK (-Z), LEFT (-X), RIGHT (+X), TOP (+Y), BOTTOM (-Y)
	pushQuad(glm::vec3(0, 0, -1),
		glm::vec3(-size, size, size), glm::vec3(size, size, size),
		glm::vec3(size, -size, size), glm::vec3(-size, -size, size), 3.0f);
	pushQuad(glm::vec3(0, 0, 1),
		glm::vec3(size, size, -size), glm::vec3(-size, size, -size),
		glm::vec3(-size, -size, -size), glm::vec3(size, -size, -size), 3.0f);
	pushQuad(glm::vec3(1, 0, 0),
		glm::vec3(-size, size, -size), glm::vec3(-size, size, size),
		glm::vec3(-size, -size, size), glm::vec3(-size, -size, -size), 3.0f);
	pushQuad(glm::vec3(-1, 0, 0),
		glm::vec3(size, size, size), glm::vec3(size, size, -size),
		glm::vec3(size, -size, -size), glm::vec3(size, -size, size), 3.0f);
	pushQuad(glm::vec3(0, -1, 0),
		glm::vec3(-size, size, -size), glm::vec3(size, size, -size),
		glm::vec3(size, size, size), glm::vec3(-size, size, size), 2.0f);
	pushQuad(glm::vec3(0, 1, 0),
		glm::vec3(-size, -size, size), glm::vec3(size, -size, size),
		glm::vec3(size, -size, -size), glm::vec3(-size, -size, -size), 1.0f);

	setupMesh();
}

void Model::AppendShadowVolumes(const glm::mat4& worldModel, const glm::vec3& lightWorld,
	std::vector<glm::vec4>& triangleVerts) const
{
	if (!hasSilhouetteData || meshTriangles.empty())
		return;

	const size_t numTri = meshTriangles.size();
	std::vector<uint8_t> litFace(numTri, 0);

	for (size_t t = 0; t < numTri; t++)
	{
		const glm::uvec3& tri = meshTriangles[t];
		glm::vec3 p0 = glm::vec3(worldModel * glm::vec4(vertices[tri.x].position, 1.f));
		glm::vec3 p1 = glm::vec3(worldModel * glm::vec4(vertices[tri.y].position, 1.f));
		glm::vec3 p2 = glm::vec3(worldModel * glm::vec4(vertices[tri.z].position, 1.f));
		glm::vec3 n = glm::normalize(glm::cross(p1 - p0, p2 - p0));
		glm::vec3 c = (p0 + p1 + p2) * (1.f / 3.f);
		glm::vec3 toLight = lightWorld - c;
		float lenTL = glm::length(toLight);
		if (lenTL < 1e-6f)
			continue;
		litFace[t] = (glm::dot(n, toLight / lenTL) > 0.f) ? 1 : 0;
	}

	auto infExt = [&](const glm::vec3& v) -> glm::vec4
	{
		return glm::vec4(v - lightWorld, 0.f);
	};

	// Front cap w=1; back cap + sides use vec4(P - L, 0) for infinite extrusion.
	auto lightBias = [&](const glm::vec3& w) -> glm::vec3
	{
		glm::vec3 d = lightWorld - w;
		float len = glm::length(d);
		if (len < 1e-6f)
			return w;
		return w + (d / len) * 0.004f;
	};

	for (size_t t = 0; t < numTri; t++)
	{
		if (!litFace[t])
			continue;
		const glm::uvec3& tri = meshTriangles[t];
		glm::vec3 w0 = glm::vec3(worldModel * glm::vec4(vertices[tri.x].position, 1.f));
		glm::vec3 w1 = glm::vec3(worldModel * glm::vec4(vertices[tri.y].position, 1.f));
		glm::vec3 w2 = glm::vec3(worldModel * glm::vec4(vertices[tri.z].position, 1.f));

		glm::vec3 f0 = lightBias(w0);
		glm::vec3 f1 = lightBias(w1);
		glm::vec3 f2 = lightBias(w2);

		triangleVerts.push_back(glm::vec4(f0, 1.f));
		triangleVerts.push_back(glm::vec4(f2, 1.f));
		triangleVerts.push_back(glm::vec4(f1, 1.f));

		triangleVerts.push_back(infExt(w1));
		triangleVerts.push_back(infExt(w2));
		triangleVerts.push_back(infExt(w0));
	}

	for (const MeshEdge& e : meshEdges)
	{
		if (e.t0 >= numTri || e.t1 >= numTri)
			continue;
		if (e.p0 >= basePositions.size() || e.p1 >= basePositions.size())
			continue;
		bool t0lit = litFace[e.t0] != 0;
		bool t1lit = litFace[e.t1] != 0;
		if (t0lit == t1lit)
			continue;

		unsigned triLit = t0lit ? e.t0 : e.t1;
		const glm::uvec3& tri = meshTriangles[triLit];

		unsigned vid[3] = { tri.x, tri.y, tri.z };
		glm::vec3 wVert[3];
		for (int k = 0; k < 3; k++)
			wVert[k] = glm::vec3(worldModel * glm::vec4(vertices[vid[k]].position, 1.f));

		int ia = -1, ib = -1;
		for (int k = 0; k < 3; k++)
		{
			if (glm::dot(vertices[vid[k]].position - basePositions[e.p0],
					vertices[vid[k]].position - basePositions[e.p0]) < 1e-8f)
				ia = k;
			if (glm::dot(vertices[vid[k]].position - basePositions[e.p1],
					vertices[vid[k]].position - basePositions[e.p1]) < 1e-8f)
				ib = k;
		}
		if (ia < 0 || ib < 0)
			continue;

		glm::vec3 wA = wVert[ia];
		glm::vec3 wB = wVert[ib];
		if ((ia + 1) % 3 != ib)
			std::swap(wA, wB);

		triangleVerts.push_back(glm::vec4(wA, 1.f));
		triangleVerts.push_back(glm::vec4(wB, 1.f));
		triangleVerts.push_back(infExt(wB));
		triangleVerts.push_back(glm::vec4(wA, 1.f));
		triangleVerts.push_back(infExt(wB));
		triangleVerts.push_back(infExt(wA));
	}
}
