#pragma once

#include "Mahakam/Core/Core.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "Material.h"

namespace Mahakam
{
	enum class ShaderSemantic
	{
		Position,
		Normal,
		Tangent,
		Color,
		TexCoord // Texcoord 0-9?
	};

	struct MeshElement
	{

	};

	class MeshLayout
	{
	private:
		std::vector<BufferElement> elements;

	public:


		std::vector<BufferElement>::iterator begin() { return elements.begin(); }
		std::vector<BufferElement>::iterator end() { return elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return elements.end(); }
	};


	class Mesh
	{
	private:
		struct Vertex
		{
			char* data;
			std::string name;
		};

		bool interleaved = true;

		char* interleavedVertices = 0;
		uint32_t vertexCount = 0;
		std::unordered_map<int, Vertex> vertices;

		uint32_t* indices = 0;
		unsigned int indexCount = 0;

		BufferLayout bufferLayout;

		Ref<VertexArray> vertexArray;

		Ref<Material> material;

		void interleaveBuffers()
		{
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

		void initBuffers(bool interleave)
		{
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

	public:
		Mesh() : vertexCount(0), bufferLayout(), indices(0), indexCount(0), material(0) {}

		Mesh(uint32_t vertexCount, const BufferLayout& layout, const uint32_t* triangles, uint32_t indexCount)
			: vertexCount(vertexCount), bufferLayout(layout), indices(new uint32_t[indexCount]), indexCount(indexCount), material(0)
		{
			memcpy(indices, triangles, indexCount * sizeof(uint32_t));
		}

		Mesh(uint32_t vertexCount, const BufferLayout& layout, const uint32_t* triangles, uint32_t indexCount, const Ref<Material>& material)
			: vertexCount(vertexCount), bufferLayout(layout), indices(new uint32_t[indexCount]), indexCount(indexCount), material(material)
		{
			memcpy(indices, triangles, indexCount * sizeof(uint32_t));
		}

		~Mesh()
		{
			for (auto& kv : vertices)
				delete[] kv.second.data;

			delete[] interleavedVertices;
		}

		void setLayout(const BufferLayout& layout)
		{
			bufferLayout = layout;
		}

		void setVertexCount(int count)
		{
			vertexCount = count;
		}

		inline void addVertices(const std::string& name, const void* verts)
		{
			int index = (int)vertices.size();

			unsigned int stride = bufferLayout.getElement(index).size;
			uint32_t size = stride * vertexCount;

			Vertex vertex
			{
				new char[size],
				name
			};

			memcpy(vertex.data, verts, size);

			vertices[index] = vertex;
		}

		void setVertices(const std::string& name, int index, const char* verts)
		{
			unsigned int stride = bufferLayout.getElement(index).size;
			uint32_t elementSize = stride * vertexCount;

			if (interleaved)
			{
				memcpy(vertices[index].data, verts, elementSize);

				interleaveBuffers();

				const Ref<VertexBuffer>& buffer = vertexArray->getVertexBuffers()[0];

				uint32_t bufferSize = bufferLayout.getStride() * vertexCount;

				buffer->setData(interleavedVertices, bufferSize);
			}
			else
			{
				auto& buffers = vertexArray->getVertexBuffers();

				buffers[index]->setData(verts, elementSize);
			}
		}

		void init(bool interleave = true)
		{
			interleaved = interleave;

			if (interleave)
				interleaveBuffers();

			initBuffers(interleave);
		}

		void setIndices(uint32_t* inds, unsigned int count)
		{
			indices = inds;
			indexCount = count;
		}

		void setMaterial(const Ref<Material>& mat)
		{
			material = mat;
		}

		template<typename T>
		inline const T& getVertices(int slot) const { return vertices[slot].data; }
		inline uint32_t getVertexSize() const { return vertexCount; }

		inline const uint32_t* getIndices() const { return indices; }
		inline uint32_t getIndexCount() const { return indexCount; }

		inline const Ref<Material>& getMaterial() const { return material; }
		inline const Ref<VertexArray>& getVertexArray() const { return vertexArray; }

		void bind()
		{
			material->bind();
			vertexArray->bind();
		}

		static Ref<Mesh> create() { return std::make_shared<Mesh>(); }

		static Ref<Mesh> create(uint32_t vertexCount, const BufferLayout& layout, const uint32_t* indices, uint32_t indexCount)
		{
			return std::make_shared<Mesh>(vertexCount, layout, indices, indexCount);
		}

		static Ref<Mesh> create(uint32_t vertexCount, const BufferLayout& layout,
			const uint32_t* indices, uint32_t indexCount, const Ref<Material>& material)
		{
			return std::make_shared<Mesh>(vertexCount, layout, indices, indexCount, material);
		}

		static Ref<Mesh> createCube(int tessellation)
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
	};
}