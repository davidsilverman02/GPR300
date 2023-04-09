#pragma once
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct DirectionalLight
{
	glm::vec3 direction;
	glm::vec3 color;
	float intensity;

	glm::vec3 safeDir()
	{
		if (direction.x == 0 && direction.y == 0 && direction.z == 0)
			return glm::vec3(1, 0, 0);
		else
			return direction;
	}
};

struct PointLight
{
	glm::vec3 position;
	glm::vec3 color;
	float intensity;
	float radius;
};

struct ConeLight
{
	glm::vec3 direction;
	glm::vec3 position;
	glm::vec3 color;
	float intensity;
	float minAngle;
	float maxAngle;
	float fallOff;
	float range;
};

struct system512
{
	const static int NUM = 8;

	DirectionalLight dLights[NUM];
	PointLight pLights[NUM];
	ConeLight cLights[NUM];
};