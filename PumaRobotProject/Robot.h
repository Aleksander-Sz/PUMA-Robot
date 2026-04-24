#ifndef ROBOT_H
#define ROBOT_H

#include <glad/gl.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include "Shader.h"
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <iostream>

class Robot
{
public:
	Robot();
	void Draw(Shader& shader);
private:
	Model robotParts[6] =
	{
		Model("Models/mesh1.txt"),
		Model("Models/mesh2.txt"),
		Model("Models/mesh3.txt"),
		Model("Models/mesh4.txt"),
		Model("Models/mesh5.txt"),
		Model("Models/mesh6.txt")
	};
};
#endif
#pragma once
