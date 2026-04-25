#include "Particles.h"

Particle::Particle(glm::vec3 origin)
{
	setupMesh();
	position = origin;
	velocity = glm::vec3(1.0f, 0.0f, 0.0f);
}

void Particle::Draw(Shader& shader)
{
	position = glm::vec3(0.0f, 2.0f, 0.0f);
	velocity = glm::vec3(1.0f, 4.0f, 1.0f);
	shader.setVec3("top", position);
	shader.setVec3("bottom", position + velocity);
	//draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Particle::Update(float deltaTime)
{
	position += velocity;
	velocity.y -= 1.0f * deltaTime;
}

void Particle::setupMesh()
{
	std::vector<int> ids;
	ids.push_back(0);
	ids.push_back(1);

	std::vector<int> indices;
	indices.push_back(0);
	indices.push_back(1);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, ids.size() * sizeof(int), &ids[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(int), (void*)0);

	glBindVertexArray(0);
}

unsigned int Particle::VBO = 0;
unsigned int Particle::VAO = 0;
unsigned int Particle::EBO = 0;

ParticleSystem::ParticleSystem()
{
	;
}

void ParticleSystem::Draw(glm::mat4 viewProjectionMatrix)
{
	shader.setMat4("viewProjection", viewProjectionMatrix);
	for (size_t i = 0; i < particles.size(); i++)
	{
		particles[i].Draw(shader);
	}
}

void ParticleSystem::Update(glm::vec3 origin)
{
	static int counter;
	counter++;
	counter = counter % 30;
	if(counter == 0)
		particles.push_back(Particle(origin));
}
