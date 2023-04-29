#include "PostProcess.h"

namespace ew
{
	ScreenMesh::ScreenMesh()
	{
		glGenVertexArrays(1, &rVAO);
		glBindVertexArray(rVAO);

		glGenBuffers(1, &rVBO);
		glBindBuffer(GL_ARRAY_BUFFER, rVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rectVerts), &rectVerts[0], GL_STATIC_DRAW);
	
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	ScreenMesh::~ScreenMesh()
	{

	}

	GLuint ScreenMesh::getVAO()
	{
		return rVAO;
	}
}