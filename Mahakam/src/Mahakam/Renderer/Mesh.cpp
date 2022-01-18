#include "mhpch.h"
#include "Mesh.h"

namespace Mahakam
{
	static Mesh* staticScreenQuad = nullptr;

	static glm::vec3 calculateCubeSphereVertex(const glm::vec3& v)
	{
		float x2 = v.x * v.x;
		float y2 = v.y * v.y;
		float z2 = v.z * v.z;
		glm::vec3 s;
		s.x = v.x * glm::sqrt(1.0f - y2 / 2.0f - z2 / 2.0f + y2 * z2 / 3.0f);
		s.y = v.y * glm::sqrt(1.0f - x2 / 2.0f - z2 / 2.0f + x2 * z2 / 3.0f);
		s.z = v.z * glm::sqrt(1.0f - x2 / 2.0f - y2 / 2.0f + x2 * y2 / 3.0f);
		return s;
	}

	static glm::vec2 calculateEquirectangularUVs(const glm::vec3& v)
	{
		const glm::vec2 invAtan = glm::vec2(0.1591, 0.3183);
		glm::vec2 uv = glm::vec2(glm::atan(v.z, v.x), glm::asin(v.y));
		uv *= invAtan;
		uv += 0.5;
		return uv;
	}

	void Mesh::interleaveBuffers()
	{
		MH_PROFILE_FUNCTION();

		uint32_t stride = bufferLayout.getStride();
		uint32_t size = stride * vertexCount;

		interleavedVertices = new char[size];

		const std::vector<BufferElement>& elements = bufferLayout.getElements();

		uint32_t dstOffset = 0;
		for (uint32_t i = 0; i < vertexCount; i++)
		{
			uint32_t srcOffset = 0;
			for (auto& vert : vertices)
			{
				uint32_t size = elements[srcOffset++].size;
				char* src = vert.second.data + i * size;
				char* dst = interleavedVertices + dstOffset;

				memcpy(dst, src, size);
				dstOffset += size;
			}
		}
	}

	void Mesh::initBuffers(bool interleave)
	{
		MH_PROFILE_FUNCTION();

		vertexArray = VertexArray::create();

		if (interleave)
		{
			Ref<VertexBuffer> vertexBuffer = VertexBuffer::create(interleavedVertices, bufferLayout.getStride() * vertexCount);
			vertexBuffer->setLayout(bufferLayout);
			vertexArray->addVertexBuffer(vertexBuffer);
		}
		else
		{
			const std::vector<BufferElement>& elements = bufferLayout.getElements();

			uint32_t offset = 0;
			for (auto& kv : vertices)
			{
				BufferLayout layout({ elements[offset] });

				Ref<VertexBuffer> vertexBuffer = VertexBuffer::create(kv.second.data, vertexCount * elements[offset].size);
				vertexBuffer->setLayout(layout);
				vertexArray->addVertexBuffer(vertexBuffer);
				offset++;
			}
		}

		Ref<IndexBuffer> indexBuffer = IndexBuffer::create(indices, indexCount);
		vertexArray->setIndexBuffer(indexBuffer);
	}

	Mesh::Mesh(uint32_t vertexCount, const BufferLayout& layout, uint32_t indexCount)
		: vertexCount(vertexCount), bufferLayout(layout), indexCount(indexCount), indices(0) {}

	Mesh::Mesh(uint32_t vertexCount, const BufferLayout& layout, const uint32_t* triangles, uint32_t indexCount)
		: vertexCount(vertexCount), bufferLayout(layout), indices(new uint32_t[indexCount]), indexCount(indexCount)
	{
		memcpy(indices, triangles, indexCount * sizeof(uint32_t));
	}

	Mesh::Mesh(uint32_t vertexCount, const BufferLayout& layout, const uint32_t* triangles, uint32_t indexCount, const std::initializer_list<void*>& verts)
		: vertexCount(vertexCount), bufferLayout(layout), indices(new uint32_t[indexCount]), indexCount(indexCount)
	{
		int offset = 0;
		int index = 0;
		for (auto& vert : verts)
		{
			const BufferElement& element = bufferLayout.getElement(index);
			uint32_t size = vertexCount * element.size;

			Vertex vertex
			{
				new char[size],
				element.name
			};

			memcpy(vertex.data, vert, size);

			vertices[index] = vertex;
			offset += size;
			index++;
		}

		memcpy(indices, triangles, indexCount * sizeof(uint32_t));

		init();
	}

	Mesh::Mesh(const Mesh& mesh)
		: vertexCount(mesh.vertexCount), bufferLayout(mesh.bufferLayout), indexCount(mesh.indexCount), vertexArray(mesh.vertexArray)
	{
		uint32_t size = vertexCount * bufferLayout.getStride();

		interleavedVertices = new char[size];
		indices = new uint32_t[indexCount];

		for (auto& kv : vertices)
			memcpy(&kv, &mesh.vertices.at(kv.first), vertexCount * bufferLayout.getElement(kv.first).size);
		memcpy(indices, mesh.indices, indexCount * sizeof(uint32_t));

		memcpy(interleavedVertices, mesh.interleavedVertices, size);
	}

	Mesh::~Mesh()
	{
		MH_PROFILE_FUNCTION();

		for (auto& kv : vertices)
			delete[] kv.second.data;

		delete[] interleavedVertices;
	}

	Mesh* Mesh::getScreenQuad()
	{
		MH_PROFILE_FUNCTION();

		if (!staticScreenQuad)
		{
			glm::vec3 positions[] = {
				{ -1.0f, 1.0f, 0.0f },
				{ 1.0f, 1.0f, 0.0f  },
				{ -1.0f, -1.0f, 0.0f },
				{ 1.0f, -1.0f, 0.0f }
			};

			glm::vec2 uvs[] = {
				{ 0.0f, 1.0f, },
				{ 1.0f, 1.0f, },
				{ 0.0f, 0.0f, },
				{ 1.0f, 0.0f }
			};

			uint32_t indices[] = {
				0, 2, 3,
				0, 3, 1
			};

			BufferLayout layout
			{
				{ ShaderDataType::Float3, "i_Pos" },
				{ ShaderDataType::Float2, "i_UV" }
			};

			staticScreenQuad = new Mesh(4, layout, indices, 6, { positions, uvs });
		}

		return staticScreenQuad;
	}

	Ref<Mesh> Mesh::createCube(int tessellation, bool reverse)
	{
		MH_PROFILE_FUNCTION();

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

		std::vector<Vertex> vertexAttributes;

		glm::vec3* positions = new glm::vec3[vertexCount];
		glm::vec2* uvs = new glm::vec2[vertexCount];
		glm::vec3* normals = new glm::vec3[vertexCount];

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
					glm::vec2 percent = { x / (float)(tessellation - 1), y / (float)(tessellation - 1) };

					glm::vec3 pointOnCube = upwards * 0.5f
						+ (percent.x - 0.5f) * axisA
						+ (percent.y - 0.5f) * axisB;

					positions[index] = pointOnCube;
					uvs[index] = percent;
					normals[index] = faces[i];

					if (x != tessellation - 1 && y != tessellation - 1)
					{
						if (reverse)
						{
							indices[triIndex] = index;
							indices[triIndex + 1] = index + tessellation;
							indices[triIndex + 2] = index + tessellation + 1;

							indices[triIndex + 3] = index;
							indices[triIndex + 4] = index + tessellation + 1;
							indices[triIndex + 5] = index + 1;
						}
						else
						{
							indices[triIndex] = index;
							indices[triIndex + 1] = index + tessellation + 1;
							indices[triIndex + 2] = index + tessellation;

							indices[triIndex + 3] = index;
							indices[triIndex + 4] = index + 1;
							indices[triIndex + 5] = index + tessellation + 1;
						}

						triIndex += 6;
					}

					index++;
				}
			}
		}

		//glm::vec3 pos = positions[0];

		BufferLayout layout
		{
			{ ShaderDataType::Float3, "i_Pos" },
			{ ShaderDataType::Float2, "i_UV" },
			{ ShaderDataType::Float3, "i_Normal" }
		};

		Ref<Mesh> mesh = Mesh::create(vertexCount, layout, indices, indexCount, { positions, uvs, normals });

		delete[] positions;
		delete[] uvs;
		delete[] normals;
		delete[] indices;

		return mesh;
	}

	Ref<Mesh> Mesh::createPlane(int rows, int columns)
	{
		MH_PROFILE_FUNCTION();

		uint32_t vertexCount = rows * columns;
		uint32_t indexCount = 6 * (rows - 1) * (columns - 1);

		glm::vec3* positions = new glm::vec3[vertexCount];
		glm::vec2* uvs = new glm::vec2[vertexCount];
		glm::vec3* normals = new glm::vec3[vertexCount];

		uint32_t* indices = new uint32_t[indexCount];

		glm::vec3 upwards = { 0.0f, 1.0f, 0.0f };
		glm::vec3 axisA(upwards.y, upwards.z, upwards.x);
		glm::vec3 axisB = glm::cross(upwards, axisA);

		int index = 0;
		int triIndex = 0;
		for (int y = 0; y < columns; y++)
		{
			for (int x = 0; x < rows; x++)
			{
				glm::vec2 percent = { x / (float)(rows - 1), y / (float)(columns - 1) };

				glm::vec3 pointOnCube = (percent.x - 0.5f) * axisA
					+ (percent.y - 0.5f) * axisB;

				positions[index] = pointOnCube;
				uvs[index] = percent;
				normals[index] = { 0.0f, 1.0f, 0.0f };

				if (x != rows - 1 && y != columns - 1)
				{
					indices[triIndex] = index;
					indices[triIndex + 1] = index + rows + 1;
					indices[triIndex + 2] = index + rows;

					indices[triIndex + 3] = index;
					indices[triIndex + 4] = index + 1;
					indices[triIndex + 5] = index + rows + 1;

					triIndex += 6;
				}

				index++;
			}
		}

		BufferLayout layout
		{
			{ ShaderDataType::Float3, "i_Pos" },
			{ ShaderDataType::Float2, "i_UV" },
			{ ShaderDataType::Float3, "i_Normal" }
		};

		Ref<Mesh> mesh = Mesh::create(vertexCount, layout, indices, indexCount, { positions, uvs, normals });

		delete[] positions;
		delete[] uvs;
		delete[] normals;
		delete[] indices;

		return mesh;
	}

	Ref<Mesh> Mesh::createUVSphere(int rows, int columns)
	{
		MH_PROFILE_FUNCTION();

		uint32_t vertexCount = rows * columns;
		uint32_t indexCount = 6 * (rows - 1) * (columns - 1);

		glm::vec3* positions = new glm::vec3[vertexCount];
		glm::vec2* uvs = new glm::vec2[vertexCount];
		glm::vec3* normals = new glm::vec3[vertexCount];

		uint32_t* indices = new uint32_t[indexCount];

		glm::vec3 upwards = { 0.0f, 1.0f, 0.0f };
		glm::vec3 axisA(upwards.y, upwards.z, upwards.x);
		glm::vec3 axisB = glm::cross(upwards, axisA);

		int index = 0;
		int triIndex = 0;
		for (int y = 0; y < columns; y++)
		{
			for (int x = 0; x < rows; x++)
			{
				glm::vec2 percent = { x / (float)(rows - 1), y / (float)(columns - 1) };

				float theta = percent.x * 2.0f * 3.1415f;
				float phi = (percent.y - 0.5f) * 3.1415f;

				if (x == rows - 1)
					theta = 0.0f;

				// This determines the radius of the ring of this line of latitude.
				// It's widest at the equator, and narrows as phi increases/decreases.
				float c = cos(phi);

				// Usual formula for a vector in spherical coordinates.
				// You can exchange x & z to wind the opposite way around the sphere.
				glm::vec3 pointOnSphere = glm::vec3(
					c * cos(theta),
					sin(phi),
					c * sin(theta)
				);

				positions[index] = pointOnSphere * 0.5f;
				uvs[index] = percent;
				normals[index] = glm::normalize(pointOnSphere);

				if (x != rows - 1 && y != columns - 1)
				{
					indices[triIndex] = index;
					indices[triIndex + 1] = index + rows + 1;
					indices[triIndex + 2] = index + rows;

					indices[triIndex + 3] = index;
					indices[triIndex + 4] = index + 1;
					indices[triIndex + 5] = index + rows + 1;

					triIndex += 6;
				}

				index++;
			}
		}

		BufferLayout layout
		{
			{ ShaderDataType::Float3, "i_Pos" },
			{ ShaderDataType::Float2, "i_UV" },
			{ ShaderDataType::Float3, "i_Normal" }
		};

		Ref<Mesh> mesh = Mesh::create(vertexCount, layout, indices, indexCount, { positions, uvs, normals });

		delete[] positions;
		delete[] uvs;
		delete[] normals;
		delete[] indices;

		return mesh;
	}

	Ref<Mesh> Mesh::createCubeSphere(int tessellation, bool equirectangular)
	{
		MH_PROFILE_FUNCTION();

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
		glm::vec3* normals = new glm::vec3[vertexCount];

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
					glm::vec2 percent = { x / (float)(tessellation - 1), y / (float)(tessellation - 1) };

					glm::vec3 pointOnCube = upwards
						+ (percent.x - 0.5f) * 2.0f * axisA
						+ (percent.y - 0.5f) * 2.0f * axisB;

					glm::vec3 pointOnSphere = calculateCubeSphereVertex(pointOnCube);

					positions[index] = pointOnSphere * 0.5f;
					if (equirectangular)
						uvs[index] = calculateEquirectangularUVs(pointOnSphere);
					else
						uvs[index] = percent;
					normals[index] = pointOnSphere;

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

		BufferLayout layout
		{
			{ ShaderDataType::Float3, "i_Pos" },
			{ ShaderDataType::Float2, "i_UV" },
			{ ShaderDataType::Float3, "i_Normal" }
		};

		Ref<Mesh> mesh = Mesh::create(vertexCount, layout, indices, indexCount, { positions, uvs, normals });

		delete[] positions;
		delete[] uvs;
		delete[] normals;
		delete[] indices;

		return mesh;
	}
}