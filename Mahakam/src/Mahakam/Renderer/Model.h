#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Profiler.h"

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Mahakam
{
	struct ModelBone
	{
		ModelBone* parent;
		glm::vec3 localPosition;
		glm::quat localRotation;
		std::vector<ModelBone> children;
		std::vector<Ref<Mesh>> meshes;
	};

	struct BoneInfo
	{
		int id;
		glm::mat4 modelToBoneSpace;
	};

	static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	class Model
	{
	private:
		ModelBone* rootBone = 0;
		std::vector<Ref<Mesh>> meshes;

		std::map<std::string, BoneInfo> m_BoneInfoMap;
		int m_BoneCounter = 0;

		Ref<Mesh> processMesh(aiMesh* mesh, const aiScene* scene)
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
				boneIDs[i] = glm::ivec4(-1);
				boneWeights[i] = glm::vec4(0.0f);
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
					if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
					{
						BoneInfo newBoneInfo;
						newBoneInfo.id = m_BoneCounter;
						newBoneInfo.modelToBoneSpace = ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
						m_BoneInfoMap[boneName] = newBoneInfo;
						boneID = m_BoneCounter;
						m_BoneCounter++;
					}
					else
					{
						boneID = m_BoneInfoMap[boneName].id;
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

		void processNode(aiNode* node, const aiScene* scene)
		{
			MH_PROFILE_FUNCTION();

			// Go through each mesh in this node
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.push_back(processMesh(mesh, scene));
			}

			// Go through any child nodes
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				processNode(node->mChildren[i], scene);
			}
		}

	public:
		Model(const std::initializer_list<Ref<Mesh>>& meshes)
			: rootBone(0), meshes(meshes) {}

		Model(const std::initializer_list<Ref<Mesh>>& meshes, ModelBone* bone)
			: rootBone(bone), meshes(meshes) {}

		Model(const std::string& filepath)
			: rootBone(0)
		{
			MH_PROFILE_FUNCTION();

			// Read model
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);

			// Process nodes in mesh
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
				MH_CORE_WARN("Could not load model \"{0}\": {1}", filepath, importer.GetErrorString());
			else
				processNode(scene->mRootNode, scene);

			//rootBone = scene->mRootNode;
		}

		~Model()
		{
			MH_PROFILE_FUNCTION();

			//delete rootBone; recursively
		}

		inline const std::vector<Ref<Mesh>>& getMeshes() const { return meshes; }

		inline auto& GetBoneInfoMap() { return m_BoneInfoMap; }
		inline int& GetBoneCount() { return m_BoneCounter; }

		static Ref<Model> load(const std::string& filepath)
		{
			return std::make_shared<Model>(filepath);
		}
	};
}