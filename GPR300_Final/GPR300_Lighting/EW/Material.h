#pragma once
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h";
#include "Transform.h"

namespace ew
{
	struct Material
	{
		Shader* shader;
		glm::vec3 color;
		float ambientK;
		float diffuseK;
		float specularK;
		float shininess;
		float normalMapIntensity;

		unsigned int text1;
		unsigned int text2;
		unsigned int normalMap;
		bool melded;

		Material()
		{
			//shader = 0;
			color = glm::vec3(0);
			ambientK = NULL;
			diffuseK = NULL;
			specularK = NULL;
			shininess = NULL;
			normalMapIntensity = NULL;
			text1 = NULL;
			text2 = NULL;
			normalMap = NULL;
			melded = NULL;
		};

		Material(Shader* sher, glm::vec3 vect, float ambient, float diffuse, float specular, float shine, float nmi, unsigned int tex1, unsigned int map)
		{
			shader = sher;
			color = vect;
			ambientK = ambient;
			diffuseK = diffuse;
			specularK = specular;
			shininess = shine;
			normalMapIntensity = nmi;
			text1 = tex1;
			text2 = NULL;
			normalMap = map;
			melded = false;
		};

		Material(Shader* sher, glm::vec3 vect, float ambient, float diffuse, float specular, float shine, float nmi, unsigned int tex1, unsigned int tex2, unsigned int map)
		{
			shader = sher;
			color = vect;
			ambientK = ambient;
			diffuseK = diffuse;
			specularK = specular;
			shininess = shine;
			normalMapIntensity = nmi;
			text1 = tex1;
			text2 = tex2;
			normalMap = map;
			melded = true;
		};

		Material(glm::vec3 vect, float ambient, float diffuse, float specular, float shine, float nmi, unsigned int tex1, unsigned int map)
		{
			shader = NULL;
			color = vect;
			ambientK = ambient;
			diffuseK = diffuse;
			specularK = specular;
			shininess = shine;
			normalMapIntensity = nmi;
			text1 = tex1;
			text2 = NULL;
			normalMap = map;
			melded = false;
		};

		Material(glm::vec3 vect, float ambient, float diffuse, float specular, float shine, float nmi, unsigned int tex1, unsigned int tex2, unsigned int map)
		{
			shader = NULL;
			color = vect;
			ambientK = ambient;
			diffuseK = diffuse;
			specularK = specular;
			shininess = shine;
			normalMapIntensity = nmi;
			text1 = tex1;
			text2 = tex2;
			normalMap = map;
			melded = true;
		};

		Material operator=(const Material& mat)
		{
			this->shader = mat.shader;
			this->color = mat.color;
			this->ambientK = mat.ambientK;
			this->diffuseK = mat.diffuseK;
			this->specularK = mat.specularK;
			this->shininess = mat.shininess;
			this->normalMapIntensity = mat.normalMapIntensity;
			this->text1 = mat.text1;
			this->text2 = mat.text2;
			this->normalMap = mat.normalMap;
			this->melded = mat.melded;
			return *this;
		}

		void bong()
		{
			shader->setVec3("material._Color", color);
		}
	};
}
