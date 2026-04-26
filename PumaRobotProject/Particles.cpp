#include "Particles.h"

Particle::Particle(glm::vec3 origin)
{
	setupMesh();
	position = origin;
	float theta = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926f;
	float phi = ((float)rand() / RAND_MAX) * 0.5f * 3.1415926f; // [0, π/2]

	float x = cos(theta) * sin(phi);
	float y = cos(phi);
	float z = sin(theta) * sin(phi);
	velocity = glm::vec3(x, y, z);
	//glm::mat4 rotation = glm::rotate(glm::rotate(), angle2, glm:vec3())
}

void Particle::Draw(Shader& shader)
{
	shader.setVec3("top", position);
	glm::vec3 bottom = position + glm::normalize(velocity) * SPARK_LENGHT;
	shader.setVec3("bottom", bottom);
	//draw mesh
	glBindVertexArray(VAO);
	glLineWidth(5);
	glDrawArrays(GL_LINES, 0, 2);
	//glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Particle::Update(float deltaTime)
{
	position += velocity * deltaTime;
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 1, GL_INT, sizeof(float), (void*)0);

	glBindVertexArray(0);
}

unsigned int Particle::VBO = 0;
unsigned int Particle::VAO = 0;
unsigned int Particle::EBO = 0;

ParticleSystem::ParticleSystem()
{
	srand(time(0));
}

void ParticleSystem::Draw(glm::mat4 viewProjectionMatrix)
{
    // make sure the shader program is active before setting uniforms / drawing
	shader.use();
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
	counter = counter % 3;
	if(counter == 0)
		particles.push_back(Particle(origin));
	double currentTime = glfwGetTime();
	double deltaTime = currentTime - lastTime;
	lastTime = currentTime;
	for (size_t i = 0; i < particles.size(); i++)
	{
		particles[i].Update(deltaTime);
	}
}
