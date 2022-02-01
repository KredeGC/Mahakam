#include "mhpch.h"
#include "Mesh.h"

namespace Mahakam
{
	static Mesh* staticScreenQuad = nullptr;
	static Mesh* staticPyramid = nullptr;
	static Mesh* staticCubemapMesh = nullptr;

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
		std::vector<BufferElement> elements;

		uint32_t size = 0;
		for (auto& vert : vertices)
		{
			size += vert.second.size;
			elements.push_back(bufferElements[vert.first]);
		}

		bufferLayout = BufferLayout(elements);

		if (interleavedVertices)
			delete[] interleavedVertices;
		interleavedVertices = new char[size];

		if (interleave)
		{
			uint32_t dstOffset = 0;
			for (uint32_t i = 0; i < vertexCount; i++)
			{
				uint32_t srcOffset = 0;
				for (auto& vert : vertices)
				{
					uint32_t size = bufferElements[srcOffset++].size;
					char* src = vert.second.data + i * size;
					char* dst = interleavedVertices + dstOffset;

					memcpy(dst, src, size);
					dstOffset += size;
				}
			}
		}
		else
		{
			uint32_t dstOffset = 0;
			for (auto& vert : vertices)
			{
				uint32_t size = vert.second.size;
				memcpy(interleavedVertices + dstOffset, (void*)vert.second.data, size);
				dstOffset += size;
			}
		}
	}

	void Mesh::initBuffers()
	{
		vertexArray = VertexArray::create(vertexCount);

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::create(interleavedVertices, bufferLayout.getStride() * vertexCount);
		vertexBuffer->setLayout(bufferLayout);
		vertexArray->addVertexBuffer(vertexBuffer, interleave);

		Ref<IndexBuffer> indexBuffer = IndexBuffer::create(indices, indexCount);
		vertexArray->setIndexBuffer(indexBuffer);
	}

	Ref<Mesh> Mesh::processMesh(SkinnedMesh& skinnedMesh, aiMesh* mesh, const aiScene* scene)
	{
		MH_PROFILE_FUNCTION();

		unsigned int numFaces = mesh->mNumFaces;

		uint32_t vertexCount = mesh->mNumVertices;

		uint32_t indexCount = 0;
		for (unsigned int i = 0; i < numFaces; i++)
			indexCount += mesh->mFaces[i].mNumIndices;

		glm::vec3* positions = new glm::vec3[mesh->mNumVertices];
		glm::vec2* texcoords = new glm::vec2[mesh->mNumVertices];
		glm::vec3* normals = new glm::vec3[mesh->mNumVertices];
		glm::vec3* tangents = new glm::vec3[mesh->mNumVertices];
		glm::vec4* colors = new glm::vec4[mesh->mNumVertices];
		glm::ivec4* boneIDs = new glm::ivec4[mesh->mNumVertices];
		glm::vec4* boneWeights = new glm::vec4[mesh->mNumVertices];

		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			boneIDs[i] = glm::ivec4(-1, -1, -1, -1);
			boneWeights[i] = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		}

		uint32_t* indices = new uint32_t[indexCount];

		// Extract vertex values
		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			if (mesh->HasPositions())
				memcpy(positions + i, mesh->mVertices + i, sizeof(glm::vec3));
			if (mesh->HasTextureCoords(0))
				memcpy(texcoords + i, mesh->mTextureCoords[0] + i, sizeof(glm::vec2));
			if (mesh->HasNormals())
				memcpy(normals + i, mesh->mNormals + i, sizeof(glm::vec3));
			if (mesh->HasTangentsAndBitangents())
				memcpy(tangents + i, mesh->mTangents + i, sizeof(glm::vec3));
			if (mesh->HasVertexColors(0))
				memcpy(colors + i, mesh->mColors[0] + i, sizeof(glm::vec4));
		}

		// Extract indices
		uint32_t indexOffset = 0;
		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			memcpy(indices + indexOffset, face.mIndices, sizeof(uint32_t) * face.mNumIndices);

			indexOffset += face.mNumIndices;
		}

		// Extract bone information
		if (mesh->HasBones())
		{
			for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
			{
				int boneID = -1;
				std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
				if (skinnedMesh.boneInfo.find(boneName) == skinnedMesh.boneInfo.end())
				{
					BoneInfo newBoneInfo;
					newBoneInfo.id = skinnedMesh.boneCount;
					newBoneInfo.offset = assimpToMat4(mesh->mBones[boneIndex]->mOffsetMatrix);
					skinnedMesh.boneInfo[boneName] = newBoneInfo;
					boneID = skinnedMesh.boneCount;
					skinnedMesh.boneCount++;
				}
				else
				{
					boneID = skinnedMesh.boneInfo[boneName].id;
				}

				MH_CORE_ASSERT(boneID != -1, "Invalid bone!");
				auto weights = mesh->mBones[boneIndex]->mWeights;
				int numWeights = mesh->mBones[boneIndex]->mNumWeights;

				for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
				{
					uint32_t vertexId = weights[weightIndex].mVertexId;
					float weight = weights[weightIndex].mWeight;
					MH_CORE_ASSERT(vertexId <= vertexCount, "Invalid vertex index!");
					//SetVertexBoneData(vertices[vertexId], boneID, weight);

					for (int i = 0; i < 4; ++i)
					{
						if (boneIDs[vertexId][i] < 0)
						{
							boneWeights[vertexId][i] = weight;
							boneIDs[vertexId][i] = boneID;
							break;
						}
					}
				}
			}

			// Normalize bone weights, because FBX is a bitch
			for (unsigned int vertIndex = 0; vertIndex < mesh->mNumVertices; ++vertIndex)
			{
				glm::vec4& weight = boneWeights[vertIndex];
				float length = weight.x + weight.y + weight.z + weight.w;

				boneWeights[vertIndex] = weight / length;
			}
		}

		Ref<Mesh> m = Mesh::create(vertexCount, indices, indexCount);
		if (mesh->HasPositions())
			m->setVertices("i_Pos", 0, (const char*)positions);
		if (mesh->HasTextureCoords(0))
			m->setVertices("i_UV", 1, (const char*)texcoords);
		if (mesh->HasNormals())
			m->setVertices("i_Normal", 2, (const char*)normals);
		if (mesh->HasTangentsAndBitangents())
			m->setVertices("i_Tangent", 3, (const char*)tangents);
		if (mesh->HasVertexColors(0))
			m->setVertices("i_Color", 4, (const char*)colors);
		if (mesh->HasBones())
		{
			m->setVertices("i_BoneIDs", 5, (const char*)boneIDs);
			m->setVertices("i_BoneWeights", 6, (const char*)boneWeights);
		}
		m->init();

		delete[] positions;
		delete[] texcoords;
		delete[] normals;
		delete[] tangents;
		delete[] colors;
		delete[] boneIDs;
		delete[] boneWeights;

		delete[] indices;

		return m;
	}

	void Mesh::processNode(SkinnedMesh& skinnedMesh, aiNode* node, const aiScene* scene)
	{
		MH_PROFILE_FUNCTION();

		// Go through each mesh in this node
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			skinnedMesh.meshes.push_back(processMesh(skinnedMesh, mesh, scene));
		}

		// Go through any child nodes
		for (uint32_t i = 0; i < node->mNumChildren; i++)
			processNode(skinnedMesh, node->mChildren[i], scene);
	}

	Mesh::Mesh(uint32_t vertexCount, uint32_t indexCount)
		: vertexCount(vertexCount), indexCount(indexCount), indices(0) {}

	Mesh::Mesh(uint32_t vertexCount, const uint32_t* triangles, uint32_t indexCount)
		: vertexCount(vertexCount), indices(new uint32_t[indexCount]), indexCount(indexCount)
	{
		memcpy(indices, triangles, indexCount * sizeof(uint32_t));
	}

	Mesh::Mesh(uint32_t vertexCount, const uint32_t* triangles, uint32_t indexCount, const std::initializer_list<void*>& verts)
		: vertexCount(vertexCount), indices(new uint32_t[indexCount]), indexCount(indexCount)
	{
		int offset = 0;
		int index = 0;
		for (auto& vert : verts)
		{
			const BufferElement& element = bufferElements[index];
			uint32_t size = vertexCount * element.size;

			Vertex vertex
			{
				new char[size],
				size,
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

	
	SkinnedMesh Mesh::loadModel(const std::string& filepath, const SkinnedMeshProps& props)
	{
		// TODO: Use SkinnedMeshProps to determine if we should load textures and create materials, or use the provided materials

		MH_PROFILE_FUNCTION();

		// Read model
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath,
			aiProcess_LimitBoneWeights |
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_ImproveCacheLocality |
			aiProcess_OptimizeMeshes |
			aiProcess_JoinIdenticalVertices);

		SkinnedMesh skinnedMesh;

		// Process nodes in mesh
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			MH_CORE_WARN("Could not load model \"{0}\": {1}", filepath, importer.GetErrorString());
		else
			processNode(skinnedMesh, scene->mRootNode, scene);

		return skinnedMesh;
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

		Ref<Mesh> mesh = Mesh::create(vertexCount, indices, indexCount, { positions, uvs, normals });

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

		Ref<Mesh> mesh = Mesh::create(vertexCount, indices, indexCount, { positions, uvs, normals });

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

		Ref<Mesh> mesh = Mesh::create(vertexCount, indices, indexCount, { positions, uvs, normals });

		delete[] positions;
		delete[] uvs;
		delete[] normals;
		delete[] indices;

		return mesh;
	}

	Ref<Mesh> Mesh::createCubeSphere(int tessellation, bool reverse, bool equirectangular)
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

		BufferLayout layout
		{
			{ ShaderDataType::Float3, "i_Pos" },
			{ ShaderDataType::Float2, "i_UV" },
			{ ShaderDataType::Float3, "i_Normal" }
		};

		Ref<Mesh> mesh = Mesh::create(vertexCount, indices, indexCount, { positions, uvs, normals });

		delete[] positions;
		delete[] uvs;
		delete[] normals;
		delete[] indices;

		return mesh;
	}
}