#ifndef PARTICLES_H
#define PARTICLES_H

#include <glad/gl.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "Shader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <iostream>
#include <fstream>
#include "Model.h"

#define SPARK_LENGHT 0.05f

class Particle
{
public:
	Particle(glm::vec3 origin, glm::mat4 normalMatrix);
	void Draw(Shader& shader);
	void Update(float deltaTime);
	bool isDead();
private:
	glm::vec3 position;
	glm::vec3 velocity;
	static unsigned int VAO, VBO, EBO;
	static void setupMesh();
	double deathTime = glfwGetTime() + 2.0f;
};

class ParticleSystem
{
public:
	ParticleSystem();
	void Draw(glm::mat4 viewProjectionMatrix);
	void Update(glm::vec3 origin, glm::mat4 transformationMatrix);
private:
	Shader shader = Shader("Shaders/ParticleVertexShader.glsl", "Shaders/ParticleFragmentShader.glsl");
	std::vector<Particle> particles;
	double lastTime = glfwGetTime();
	unsigned int particleLimit = 500;
};

#endif
#pragma once
