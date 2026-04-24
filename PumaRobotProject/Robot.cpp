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
		robotParts[i].model = modelMatrices[i];
		robotParts[i].Draw(shader);
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

void Robot::InverseKinematics(glm::vec3 position, glm::vec3 normal)
{
	float l1 = .91f, l2 = .81f, l3 = .33f, dy = .27f, dz = .26f;
	normal = normalize(normal);
	glm::vec3 pos1 = position + normal * l3;
	float e = sqrtf(pos1.z * pos1.z + pos1.x * pos1.x - dz * dz);
	float a1 = atan2(pos1.z, -pos1.x) + atan2(dz, e);
	glm::vec3 pos2(e, pos1.y - dy, .0f);
	float a3 = -acosf(std::min(1.0f, (pos2.x * pos2.x + pos2.y * pos2.y - l1 * l1 - l2 * l2)
		/ (2.0f * l1 * l2)));
	float k = l1 + l2 * cosf(a3), l = l2 * sinf(a3);
	float a2 = -atan2(pos2.y, sqrtf(pos2.x * pos2.x + pos2.z * pos2.z)) - atan2(l, k);
	glm::vec3 normal1;
	normal1 = glm::vec3(glm::rotate(glm::mat4(1.0f), -a1, glm::vec3(0, 1, 0)) *
		glm::vec4(normal.x, normal.y, normal.z, .0f));
	normal1 = glm::vec3(glm::rotate(glm::mat4(1.0f), -(a2 + a3), glm::vec3(0, 0, 1)) *
		glm::vec4(normal1.x, normal1.y, normal1.z, .0f));
	float a5 = acosf(normal1.x);
	float a4 = atan2(normal1.z, normal1.y);


	jointAngles[1] = a1;
	jointAngles[2] = a2;
	jointAngles[3] = a3;
	jointAngles[4] = a4;
	jointAngles[5] = a5;
}