#pragma once
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct LightProperties
{
	glm::vec3 color;
	float ambientK;
	float diffuseK;
	float specularK;
	float shininess;
	float normalMapIntensity;
};