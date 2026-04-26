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

#define SPARK_LENGHT 0.1f

class Particle
{
public:
	Particle(glm::vec3 origin);
	void Draw(Shader& shader);
	void Update(float deltaTime);
private:
	glm::vec3 position;
	glm::vec3 velocity;
	static unsigned int VAO, VBO, EBO;
	static void setupMesh();
};

class ParticleSystem
{
public:
	ParticleSystem();
	void Draw(glm::mat4 viewProjectionMatrix);
	void Update(glm::vec3 origin);
private:
	Shader shader = Shader("Shaders/ParticleVertexShader.glsl", "Shaders/ParticleFragmentShader.glsl");
	std::vector<Particle> particles;
	double lastTime = glfwGetTime();
};

#endif
#pragma once
