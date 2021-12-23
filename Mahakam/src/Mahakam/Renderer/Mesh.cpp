#include "mhpch.h"
#include "Mesh.h"

namespace Mahakam
{
	Ref<Mesh> Mesh::createCube(int tessellation)
	{
		uint32_t vertexCount = 6 * tessellation * tessellation;
		uint32_t indexCount = 6 * 6 * (tessellation - 1) * (tessellation - 1);

		glm::vec3 faces[6]
		{
			{  1.0f,  0.0f,  0.0f },
			{  0.0f,  1.0f,  0.0f },
			{  0.0f,  0.0f,  1.0f },
			{ -1.0f,  0.0f,  0.0f },
			{  0.0f, -1.0f,  0.0f },
			{  0.0f,  0.0f, -1.0f }
		};

		glm::vec3* positions = new glm::vec3[vertexCount];

		glm::vec2* uvs = new glm::vec2[vertexCount];

		uint32_t* indices = new uint32_t[indexCount];

		int index = 0;
		int triIndex = 0;
		for (int i = 0; i < 6; i++)
		{
			glm::vec3 upwards = faces[i];
			glm::vec3 axisA(upwards.y, upwards.z, upwards.x);
			glm::vec3 axisB = glm::cross(upwards, axisA);

			for (int y = 0; y < tessellation; y++)
			{
				for (int x = 0; x < tessellation; x++)
				{
					glm::vec2 percent = { x / (tessellation - 1), y / (tessellation - 1) };

					glm::vec3 pointOnCube = upwards * 0.5f
						+ (percent.x - 0.5f) * axisA
						+ (percent.y - 0.5f) * axisB;

					positions[index] = pointOnCube;
					uvs[index] = percent;

					if (x != tessellation - 1 && y != tessellation - 1)
					{
						indices[triIndex] = index;
						indices[triIndex + 1] = index + tessellation + 1;
						indices[triIndex + 2] = index + tessellation;

						indices[triIndex + 3] = index;
						indices[triIndex + 4] = index + 1;
						indices[triIndex + 5] = index + tessellation + 1;

						triIndex += 6;
					}

					index++;
				}
			}
		}

		//glm::vec3 pos = positions[0];

		BufferLayout layout
		{
			{ ShaderDataType::Float3, "i_Pos"},
			{ ShaderDataType::Float2, "i_UV"},
			//{ ShaderDataType::Float3, "i_Normal"}
		};

		Ref<Mesh> mesh = Mesh::create(vertexCount, layout, indices, indexCount);
		mesh->addVertices("i_Pos", positions);
		mesh->addVertices("i_UV", uvs);
		mesh->init();

		//delete[] positions;
		//delete[] uvs;
		//delete[] indices;

		return mesh;
	}
}