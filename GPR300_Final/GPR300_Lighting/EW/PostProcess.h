#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

namespace ew
{
	float rectVerts[]
	{
		 1.0f, -1.0f,   1.0f, 0.0f,
		-1.0f, -1.0f,   0.0f, 0.0f,
		-1.0f,  1.0f,   0.0f, 1.0f,

		 1.0f,  1.0f,   1.0f, 1.0f,
		 1.0f, -1.0f,   1.0f, 0.0f,
		-1.0f,  1.0f,   0.0f, 1.0f
	};

	class ScreenMesh {
	public:
		ScreenMesh();
		~ScreenMesh();
		void draw();
		GLuint getVAO();
	private:
		GLuint rVAO, rVBO;
	};


}