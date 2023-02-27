#pragma once
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct DirectionalLight
{
	glm::vec3 direction;
	glm::vec3 color;
	float intensity;
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