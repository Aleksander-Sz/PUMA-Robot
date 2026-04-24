#include "Robot.h"

Robot::Robot()
{
	;
}

void Robot::Draw(Shader& shader)
{
	for (size_t i = 0; i < 6; i++)
	{
		robotParts[i].Draw(shader);
	}
}