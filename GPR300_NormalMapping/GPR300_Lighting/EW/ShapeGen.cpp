//Author: Eric Winebrenner

#include "ShapeGen.h"
#include <glm/gtc/type_ptr.hpp>

namespace ew {
    glm::vec3 quickTangent(glm::vec3 basePos0, glm::vec3 basePos1, glm::vec3 basePos2, glm::vec2 uv0, glm::vec2 uv1, glm::vec2 uv2)
	{
		glm::vec3 dPos1 = basePos1 - basePos0;
		glm::vec3 dPos2 = basePos2 - basePos0;

		glm::vec2 dUV1 = uv1 - uv0;
		glm::vec2 dUV2 = uv2 - uv0;

		float r = 1.0f / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);

		return (dPos1 * dUV2.y - dPos2 * dUV1.y) * r;
	}

	glm::vec3 quickTangent(glm::vec3 basePos0, glm::vec3 basePos1, glm::vec3 basePos2, glm::vec3 basePos3, glm::vec2 uv0, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, bool front)
	{
		if (front)
			return (quickTangent(basePos0, basePos2, basePos1, uv0, uv2, uv1) + quickTangent(basePos0, basePos3, basePos2, uv0, uv3, uv2)) / 2.0f;
		else
			return (quickTangent(basePos0, basePos1, basePos2, uv0, uv1, uv2) + quickTangent(basePos2, basePos3, basePos0, uv2, uv3, uv0)) / 2.0f;
	}

	bool confirm(Vertex vr, Vertex v1, Vertex v2, Vertex v3)
	{
		return vr == v1 || vr == v2 || vr == v3;
	}

	void indexSphere(std::vector<Vertex> ver, int num, int segments)
	{
		unsigned int topIndex = 0;
		unsigned int bottomIndex = num - 1;
		unsigned int ringVertexCount = segments + 1;

		unsigned int start;

		float amount;

		glm::vec3 topNormal;

		for (int i = 0; i < num; i++)
		{
			amount = 0;

			topNormal = glm::vec3(0, 0, 0);

			for (int j = 0; j < segments; ++j) {
				if (confirm(ver[i], ver[topIndex], ver[j + 1], ver[j + 2]))
				{
					topNormal += quickTangent(ver[topIndex].position, ver[j + 1].position, ver[j + 2].position, ver[topIndex].uv, ver[j + 1].uv, ver[j + 2].uv);
					amount++;
				}
			}

			start = 1;

			for (int y = 0; y < segments - 2; ++y)
			{
				for (int x = 0; x < segments; ++x)
				{
					if (confirm(ver[i], ver[start + y * ringVertexCount + x], ver[start + (y + 1) * ringVertexCount + x], ver[start + y * ringVertexCount + x + 1]))
					{
						topNormal += quickTangent(ver[start + y * ringVertexCount + x].position, ver[start + (y + 1) * ringVertexCount + x].position, ver[start + y * ringVertexCount + x + 1].position, ver[start + y * ringVertexCount + x].uv, ver[start + (y + 1) * ringVertexCount + x].uv, ver[start + y * ringVertexCount + x + 1].uv);
						amount++;
					}

					if (confirm(ver[i], ver[start + y * ringVertexCount + x + 1], ver[start + (y + 1) * ringVertexCount + x], ver[start + (y + 1) * ringVertexCount + x + 1]))
					{
						topNormal += quickTangent(ver[start + y * ringVertexCount + x + 1].position, ver[start + (y + 1) * ringVertexCount + x].position, ver[start + (y + 1) * ringVertexCount + x + 1].position, ver[start + y * ringVertexCount + x + 1].uv, ver[start + (y + 1) * ringVertexCount + x].uv, ver[start + (y + 1) * ringVertexCount + x + 1].uv);
						amount++;
					}
				}
			}

			start = bottomIndex - ringVertexCount;

			for (int k = 0; k < ringVertexCount; ++k) {
				if (confirm(ver[i], ver[start + k + 1], ver[start + k], ver[bottomIndex]))
				{
					topNormal += quickTangent(ver[start + k + 1].position, ver[start + k].position, ver[bottomIndex].position, ver[start + k + 1].uv, ver[start + k].uv, ver[bottomIndex].uv);
					amount++;
				}
			}

			if (amount != 0)
				ver[i].setTangent(topNormal / amount);
		}
	}

	void indexCylinder(std::vector<Vertex> ver, int num, int segments, int bci, int ssi)
	{
		float amount;

		unsigned int start;

		glm::vec3 topNormal;

		for (int i = 0; i < num; i++)
		{
			amount = 0;

			start = 0;

			topNormal = glm::vec3(0, 0, 0);

			for (int j = 0; j < segments; ++j) {
				if (confirm(ver[i], ver[j + 1], ver[0], ver[j + 2]))
				{
					topNormal += quickTangent(ver[j + 1].position, ver[0].position, ver[j + 2].position, ver[j + 1].uv, ver[0].uv, ver[j + 2].uv);
					amount++;
				}
			}

			for (int k = 0; k < segments; ++k) {
				if (confirm(ver[i], ver[bci], ver[bci + k + 1], ver[bci + k + 2]))
				{
					topNormal += quickTangent(ver[bci].position, ver[bci + k + 1].position, ver[bci + k + 2].position, ver[bci].uv, ver[bci + k +1].uv, ver[bci].uv);
					amount++;
				}
			}

			for (int y = 0; y < segments; ++y)
			{
				start = ssi + y;

				if (confirm(ver[i], ver[start], ver[start + 1], ver[start + segments + 1]))
				{
					topNormal += quickTangent(ver[start].position, ver[start + 1].position, ver[start + segments + 1].position, ver[start].uv, ver[start + 1].uv, ver[start + segments + 1].uv);
					amount++;
				}

				if (confirm(ver[i], ver[start + segments + 1], ver[start], ver[start + segments + 2]))
				{
					topNormal += quickTangent(ver[start + segments + 1].position, ver[start].position, ver[start + segments + 2].position, ver[start + segments + 1].uv, ver[start].uv, ver[start + segments + 2].uv);
					amount++;
				}
			}

			if (amount != 0)
				ver[i].setTangent(topNormal / amount);
		}
	}

	void createPlane(float width, float height, MeshData& meshData) {
		meshData.vertices.clear();
		meshData.indices.clear();
		float halfWidth = width / 2.0f;
		float halfHeight = height / 2.0f;
		Vertex vertices[4] = {
			//Front face
			{glm::vec3(-halfWidth, 0, -halfHeight), glm::vec3(0,1,0), glm::vec2(0, 0)}, //BL
			{glm::vec3(+halfWidth, 0, -halfHeight), glm::vec3(0,1,0), glm::vec2(1, 0)}, //BR
			{glm::vec3(+halfWidth, 0, +halfHeight), glm::vec3(0,1,0), glm::vec2(1, 1)}, //TR
			{glm::vec3(-halfWidth, 0, +halfHeight), glm::vec3(0,1,0), glm::vec2(0, 1)} //TL
		};
		meshData.vertices.assign(&vertices[0], &vertices[4]);
		glm::vec3 find = quickTangent(vertices[0].position, vertices[1].position, vertices[2].position, vertices[3].position, vertices[0].uv, vertices[1].uv, vertices[2].uv, vertices[3].uv, true);

		vertices[0].setTangent(find);
		vertices[1].setTangent(find);
		vertices[2].setTangent(find);
		vertices[3].setTangent(find);

		unsigned int indices[6] = {
			// front face
			0, 2, 1,
			0, 3, 2
		};
		meshData.indices.assign(&indices[0], &indices[6]);
	};

	void createQuad(float width, float height, MeshData& meshData) {
		meshData.vertices.clear();
		meshData.indices.clear();
		float halfWidth = width / 2.0f;
		float halfHeight = height / 2.0f;
		Vertex vertices[4] = {
			//Front face
			{glm::vec3(-halfWidth, -halfHeight, 0), glm::vec3(0,0,1)}, //BL
			{glm::vec3(+halfWidth, -halfHeight, 0), glm::vec3(0,0,1)}, //BR
			{glm::vec3(+halfWidth, +halfHeight, 0), glm::vec3(0,0,1)}, //TR
			{glm::vec3(-halfWidth, +halfHeight, 0), glm::vec3(0,0,1)} //TL
		};
		meshData.vertices.assign(&vertices[0], &vertices[4]);
		unsigned int indices[6] = {
			// front face
			0, 1, 2,
			0, 2, 3
		};
		meshData.indices.assign(&indices[0], &indices[6]);
	};

	void createCube(float width, float height, float depth, MeshData& meshData)
	{
		meshData.vertices.clear();
		meshData.indices.clear();

		float halfWidth = width / 2.0f;
		float halfHeight = height / 2.0f;
		float halfDepth = depth / 2.0f;

		//VERTICES
		//-------------
		Vertex vertices[24] = {
			//Front face
			{glm::vec3(-halfWidth, -halfHeight, +halfDepth), glm::vec3(0,0,1), glm::vec2(0, 0)}, //BL
			{glm::vec3(+halfWidth, -halfHeight, +halfDepth), glm::vec3(0,0,1), glm::vec2(1, 0)}, //BR
			{glm::vec3(+halfWidth, +halfHeight, +halfDepth), glm::vec3(0,0,1), glm::vec2(1, 1)}, //TR
			{glm::vec3(-halfWidth, +halfHeight, +halfDepth), glm::vec3(0,0,1), glm::vec2(0, 1)}, //TL

			//Back face
			{glm::vec3(+halfWidth, -halfHeight, -halfDepth), glm::vec3(0,0,-1), glm::vec2(1, 0)}, //BL
			{glm::vec3(-halfWidth, -halfHeight, -halfDepth), glm::vec3(0,0,-1), glm::vec2(0, 0)}, //BR
			{glm::vec3(-halfWidth, +halfHeight, -halfDepth), glm::vec3(0,0,-1), glm::vec2(0, 1)}, //TR
			{glm::vec3(+halfWidth, +halfHeight, -halfDepth), glm::vec3(0,0,-1), glm::vec2(1, 1)}, //TL

			//Right face
			{glm::vec3(+halfWidth, -halfHeight, +halfDepth), glm::vec3(1,0,0), glm::vec2(0, 1)}, //BL
			{glm::vec3(+halfWidth, -halfHeight, -halfDepth), glm::vec3(1,0,0), glm::vec2(0, 0)}, //BR
			{glm::vec3(+halfWidth, +halfHeight, -halfDepth), glm::vec3(1,0,0), glm::vec2(1, 0)}, //TR
			{glm::vec3(+halfWidth, +halfHeight, +halfDepth), glm::vec3(1,0,0), glm::vec2(1, 1)}, //TL

			//Left face
			{glm::vec3(-halfWidth, -halfHeight, -halfDepth), glm::vec3(-1,0,0), glm::vec2(0, 0)}, //BL
			{glm::vec3(-halfWidth, -halfHeight, +halfDepth), glm::vec3(-1,0,0), glm::vec2(0, 1)}, //BR
			{glm::vec3(-halfWidth, +halfHeight, +halfDepth), glm::vec3(-1,0,0), glm::vec2(1, 1)}, //TR
			{glm::vec3(-halfWidth, +halfHeight, -halfDepth), glm::vec3(-1,0,0), glm::vec2(1, 0)}, //TL

			//Top face
			{glm::vec3(-halfWidth, +halfHeight, +halfDepth), glm::vec3(0,1,0), glm::vec2(0, 1)}, //BL
			{glm::vec3(+halfWidth, +halfHeight, +halfDepth), glm::vec3(0,1,0), glm::vec2(1, 1)}, //BR
			{glm::vec3(+halfWidth, +halfHeight, -halfDepth), glm::vec3(0,1,0), glm::vec2(1, 0)}, //TR
			{glm::vec3(-halfWidth, +halfHeight, -halfDepth), glm::vec3(0,1,0), glm::vec2(0, 0)}, //TL

			//Bottom face
			{glm::vec3(-halfWidth, -halfHeight, -halfDepth), glm::vec3(0,-1,0), glm::vec2(0, 0)}, //BL
			{glm::vec3(+halfWidth, -halfHeight, -halfDepth), glm::vec3(0,-1,0), glm::vec2(1, 0)}, //BR
			{glm::vec3(+halfWidth, -halfHeight, +halfDepth), glm::vec3(0,-1,0), glm::vec2(1, 1)}, //TR
			{glm::vec3(-halfWidth, -halfHeight, +halfDepth), glm::vec3(0,-1,0), glm::vec2(0, 1)}, //TL
		};
		
		glm::vec3 find;

		for (int i = 0; i < 6; i++)
		{
			if (i == 0)
				find = quickTangent(vertices[(i * 4)].position, vertices[(i * 4) + 1].position, vertices[(i * 4) + 2].position, vertices[(i * 4) + 3].position, vertices[(i * 4)].uv, vertices[(i * 4) + 1].uv, vertices[(i * 4) + 2].uv, vertices[(i * 4) + 3].uv, true);
			else
				find = quickTangent(vertices[(i * 4)].position, vertices[(i * 4) + 1].position, vertices[(i * 4) + 2].position, vertices[(i * 4) + 3].position, vertices[(i * 4)].uv, vertices[(i * 4) + 1].uv, vertices[(i * 4) + 2].uv, vertices[(i * 4) + 3].uv, false);

			vertices[(i * 4)].setTangent(find);
			vertices[(i * 4) + 1].setTangent(find);
			vertices[(i * 4) + 2].setTangent(find);
			vertices[(i * 4) + 3].setTangent(find);
		}

		meshData.vertices.assign(&vertices[0], &vertices[24]);

		//INDICES
		//-------------

		unsigned int indices[36] = {
			// front face
			0, 1, 2,
			0, 2, 3,

			// back face
			4, 5, 6,
			6, 7, 4,

			// right face
			8,  9, 10,
			10, 11, 8,

			//left face 
			12, 13, 14,
			14, 15, 12,

			//top face
			16,17,18,
			18,19,16,

			//bottom face
			20, 21, 22,
			22, 23, 20
		};
		meshData.indices.assign(&indices[0], &indices[36]);
	}

	void createSphere(float radius, int numSegments, MeshData& meshData)
	{
		meshData.vertices.clear();
		meshData.indices.clear();

		float topY = radius;
		float bottomY = -radius;

		unsigned int topIndex = 0;
		meshData.vertices.push_back({ glm::vec3(0,topY,0),glm::vec3(0,1,0), glm::vec2((cos(0) + 1) / 2, (sin(0) + 1) / 2) });

		//Angle between segments
		float thetaStep = (2.0f * glm::pi<float>()) / (float)numSegments;
		float phiStep = (glm::pi<float>()) / (float)numSegments;

		for (int i = 1; i < numSegments; i++)
		{
			float phi = phiStep * i;

			//Create row
			for (int j = 0; j <= numSegments; ++j)
			{
				float theta = thetaStep * j;

				float x = radius * sinf(phi) * sinf(theta);
				float y = radius * cosf(phi);
				float z = radius * sinf(phi) * cosf(theta);

				glm::vec3 position = glm::vec3(x, y, z);
				glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
				//glm::vec2 uv = glm::vec2(i / (float)(numSegments), j / (float)(numSegments));
				glm::vec2 uv = glm::vec2((cos(phi) + 1) / 2, (sin(theta) + 1) / 2);

				meshData.vertices.push_back({ position, normal, uv });
			}
		}

		meshData.vertices.push_back({ glm::vec3(0,bottomY,0), glm::vec3(0,-1,0), glm::vec2((cos(1) + 1) / 2, (sin(1) + 1) / 2) });

		indexSphere(meshData.vertices, meshData.vertices.size(), numSegments);

		unsigned int bottomIndex = (unsigned int)meshData.vertices.size() - 1;
		unsigned int ringVertexCount = numSegments + 1;

		//TOP CAP
		for (int i = 0; i < numSegments; ++i) {
			meshData.indices.push_back(topIndex); //top cap center 
			meshData.indices.push_back(i + 1);
			meshData.indices.push_back(i + 2);
		}

		//RINGS
		unsigned int start = 1;

		//Row index
		//-2 to ignore poles
		for (int y = 0; y < numSegments - 2; ++y)
		{
			//Column index
			for (int x = 0; x < numSegments; ++x)
			{
				//Triangle 1
				meshData.indices.push_back(start + y * ringVertexCount + x);
				meshData.indices.push_back(start + (y + 1) * ringVertexCount + x);
				meshData.indices.push_back(start + y * ringVertexCount + x + 1);

				//Triangle 2
				meshData.indices.push_back(start + y * ringVertexCount + x + 1);
				meshData.indices.push_back(start + (y + 1) * ringVertexCount + x);
				meshData.indices.push_back(start + (y + 1) * ringVertexCount + x + 1);
			}
		}

		start = bottomIndex - ringVertexCount;

		//BOTTOM CAP
		for (unsigned int i = 0; i < ringVertexCount; ++i) {
			meshData.indices.push_back(start + i + 1);
			meshData.indices.push_back(start + i);
			meshData.indices.push_back(bottomIndex); //bottom cap center 
		}
	}

	void createCylinder(float height, float radius, int numSegments, MeshData& meshData)
	{
		meshData.vertices.clear();
		meshData.indices.clear();

		float halfHeight = height * 0.5f;
		float thetaStep = glm::pi<float>() * 2.0f / numSegments;
		float phiStep = (glm::pi<float>()) / (float)numSegments;

		//VERTICES
		//Top cap (facing up)
		meshData.vertices.push_back({ glm::vec3(0, halfHeight, 0), glm::vec3(0, 1, 0), glm::vec2(0.5, 0.5) });
		for (int i = 0; i <= numSegments; i++)
		{
			float theta = thetaStep * i;

			glm::vec3 pos = glm::vec3(
				cos(i * thetaStep) * radius,
				halfHeight,
				sin(i * thetaStep) * radius
			);

			glm::vec2 uv = glm::vec2((cos(theta) + 1) / 2, (sin(theta) + 1) / 2);

			meshData.vertices.push_back({ pos, glm::vec3(0, 1, 0), uv });
		}

		//Bottom cap (facing down)
		meshData.vertices.push_back({ glm::vec3(0, -halfHeight, 0), glm::vec3(0, -1, 0), glm::vec2(0.5, 0.5) });
		unsigned int bottomCenterIndex = (unsigned int)meshData.vertices.size() - 1;
		for (int i = 0; i <= numSegments; i++)
		{
			float theta = thetaStep * i;

			glm::vec3 pos = glm::vec3(
				cos(i * thetaStep) * radius,
				-halfHeight,
				sin(i * thetaStep) * radius
			);

			glm::vec2 uv = glm::vec2((cos(theta) + 1 )/ 2, (sin(theta) + 1 )/ 2);

			meshData.vertices.push_back({ pos, glm::vec3(0, -1, 0), uv });
		}

		//Sides (facing out)
		unsigned int sideStartIndex = (unsigned int)meshData.vertices.size();
		//Side top ring
		for (int i = 0; i <= numSegments; i++)
		{
			glm::vec3 pos = meshData.vertices[i + 1].position;
			glm::vec3 normal = glm::normalize((pos - meshData.vertices[0].position));

			glm::vec2 uv = glm::vec2(i / (float)(numSegments), 1);

			meshData.vertices.push_back({ pos, normal, uv });
		}
		//Side bottom ring
		for (int i = 0; i <= numSegments; i++)
		{
			glm::vec3 pos = meshData.vertices[bottomCenterIndex + i + 1].position;
			glm::vec3 normal = glm::normalize((pos - meshData.vertices[bottomCenterIndex].position));

			glm::vec2 uv = glm::vec2(i/(float)(numSegments), 0);
			meshData.vertices.push_back({ pos, normal , uv});
		}

		indexCylinder(meshData.vertices, meshData.vertices.size(), numSegments, bottomCenterIndex, sideStartIndex);

		//INDICES
		//Top cap
		for (int i = 0; i < numSegments; i++)
		{
			meshData.indices.push_back(i + 1);
			meshData.indices.push_back(0);
			meshData.indices.push_back(i + 2);
		}
		//Bottom cap
		for (int i = 0; i < numSegments; i++)
		{
			meshData.indices.push_back(bottomCenterIndex);
			meshData.indices.push_back(bottomCenterIndex + i + 1);
			meshData.indices.push_back(bottomCenterIndex + i + 2);
		}
		//Side quads
		for (int i = 0; i < numSegments; i++)
		{
			unsigned int start = sideStartIndex + i;
			meshData.indices.push_back(start);
			meshData.indices.push_back(start + 1);
			meshData.indices.push_back(start + numSegments + 1);
			meshData.indices.push_back(start + numSegments + 1);
			meshData.indices.push_back(start + 1);
			meshData.indices.push_back(start + numSegments + 2);
		}
	}

}
