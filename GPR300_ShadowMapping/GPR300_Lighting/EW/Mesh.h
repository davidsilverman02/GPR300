//Author: Eric Winebrenner

#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

namespace ew {
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;
		glm::vec3 tangent;
		Vertex(glm::vec3 position, glm::vec3 normal)
			: position(position), normal(normal) {};
		Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv)
			: position(position), normal(normal), uv(uv) {};
		Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv, glm::vec3 tangent)
			: position(position), normal(normal), uv(uv), tangent(tangent) {};

		void setTangent(glm::vec3 tange) 
		{
			tangent = tange;
		};

		bool operator==(const Vertex& v)
		{
			if (this->position == v.position && this->normal == v.normal && this->uv == v.uv)
				return true;
			else
				return false;
		}

		bool inTri(Vertex a, Vertex b, Vertex c)
		{
		
		}
	};

	/// <summary>
	/// Just holds a bunch of vertex + face (indices) data
	/// </summary>
	struct MeshData {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};

	/// <summary>
	/// Holds OpenGL buffers, can be drawn
	/// </summary>
	class Mesh {
	public:
		Mesh(MeshData* meshData);
		~Mesh();
		void draw();
	private:
		GLuint mVAO, mVBO, mEBO;
		GLsizei mNumIndices;
		GLsizei mNumVertices;
	};
}
