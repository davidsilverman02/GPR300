//Author: Eric Winebrenner

#include "Mesh.h"

namespace ew {
	Mesh::Mesh(MeshData* meshData) {

		glGenVertexArrays(1, &mVAO);
		glBindVertexArray(mVAO);

		glGenBuffers(1, &mVBO);
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, meshData->vertices.size() * sizeof(Vertex), &meshData->vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &mEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData->indices.size() * sizeof(unsigned int), &meshData->indices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, position)));
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, normal)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, uv)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, tangent)));
		glEnableVertexAttribArray(3);

		mNumIndices = (GLsizei)meshData->indices.size();
		mNumVertices = (GLsizei)meshData->vertices.size();
	}

	Mesh::~Mesh()
	{
		glDeleteVertexArrays(1, &mVAO);
		glDeleteBuffers(1, &mVBO);
		glDeleteBuffers(1, &mEBO);
	}

	void Mesh::draw()
	{
		glBindVertexArray(mVAO);
		glDrawElements(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, 0);
	}

	void Mesh::materialDraw(Transform tras, unsigned int text)
	{
		matter.shader->use();

		matter.shader->setMat4("_Model", tras.getModelMatrix());
		matter.shader->setVec3("material._Color", matter.color);
		matter.shader->setFloat("material._AmbientK", matter.ambientK);
		matter.shader->setFloat("material._DiffuseK", matter.diffuseK);
		matter.shader->setFloat("material._SpecularK", matter.specularK);
		matter.shader->setFloat("material._Shininess", matter.shininess);
		matter.shader->setFloat("material._normalMapIntensity", matter.normalMapIntensity);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, matter.text1);

		if (matter.melded)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, matter.text2);
		}

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, matter.normalMap);

		//glActiveTexture(GL_TEXTURE3);
		//glBindTexture(GL_TEXTURE_2D, text);

		matter.shader->setInt("_SandTexture", 0);
		matter.shader->setInt("_BrickTexture", 1);
		matter.shader->setInt("_MesoTexture", 2);
		//matter.shader->setInt("_ShadowTexture", 3);


		//matter.shader->setMat4("_Model", tras.getModelMatrix());

		//draw();

		glBindVertexArray(mVAO);
		glDrawElements(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, 0);
	}

}
