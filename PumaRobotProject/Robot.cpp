#include "Robot.h"

Robot::Robot()
{
	for (size_t i = 0; i < 6; i++)
	{
		modelMatrices[i] = glm::mat4(1.0f);
	}
}

void Robot::Draw(Shader& shader)
{
	RotateJoints();
	for (size_t i = 0; i < 6; i++)
	{
		shader.setMat4("model", modelMatrices[i]);
		robotParts[i].Draw(shader);
		jointAngles[i] += 0.001f;
	}
}

void Robot::RotateJoints()
{
	modelMatrices[1] = glm::rotate(glm::mat4(1.0f), jointAngles[1], glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrices[2] = modelMatrices[1] * glm::translate(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.27f, 0.0f)), jointAngles[2], glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(0.0f, -0.27f, 0.0f));
	modelMatrices[3] = modelMatrices[2] * glm::translate(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-0.97f, 0.27f, 0.0f)), jointAngles[3], glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(0.97f, -0.27f, 0.0f));
	modelMatrices[4] = modelMatrices[3] * glm::translate(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.27f, -0.26f)), jointAngles[4], glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(0.0f, -0.27f, 0.26f));
	modelMatrices[5] = modelMatrices[4] * glm::translate(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-1.72f, 0.27f, 0.0f)), jointAngles[5], glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(1.72f, -0.27f, 0.0f));
}