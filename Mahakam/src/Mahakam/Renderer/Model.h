#pragma once

#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/Log.h"

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Mahakam
{
	struct ModelBone
	{
		ModelBone* parent;
		std::vector<ModelBone> children;
		std::vector<Ref<Mesh>> meshes;
	};

	class Model
	{
	private:
		struct Vertex
		{
			char* data;
			std::string name;
		};

		ModelBone* rootBone = 0;
		std::vector<Ref<Mesh>> meshes;

		MeshLayout meshLayout;

		Ref<Mesh> processMesh(aiMesh* mesh, const aiScene* scene)
		{
			MH_PROFILE_FUNCTION();

			unsigned int numFaces = mesh->mNumFaces;

			uint32_t vertexCount = mesh->mNumVertices;

			uint32_t indexCount = 0;
			for (unsigned int i = 0; i < numFaces; i++)
				indexCount += mesh->mFaces[i].mNumIndices;

			std::vector<Vertex> vertices;
			uint32_t* indices = new uint32_t[indexCount];

			for (auto& element : meshLayout)
			{
				uint32_t size = ShaderSemanticSize(element.semantic);

				Vertex vertex
				{
					new char[size * vertexCount],
					element.name
				};

				// Choose src pointer
				char* srcArray = nullptr;
				int srcSize = 0;
				switch (element.semantic)
				{
				case ShaderSemantic::Position:
					srcArray = (char*)mesh->mVertices;
					srcSize = sizeof(aiVector3D);
					break;
				case ShaderSemantic::Normal:
					srcArray = (char*)mesh->mNormals;
					srcSize = sizeof(aiVector3D);
					break;
				case ShaderSemantic::Tangent:
					srcArray = (char*)mesh->mTangents;
					srcSize = sizeof(aiVector3D);
					break;
				case ShaderSemantic::Color:
					srcArray = (char*)mesh->mTangents;
					srcSize = sizeof(aiColor4D);
					break;
				case ShaderSemantic::TexCoord0:
					srcArray = (char*)mesh->mTextureCoords[0];
					srcSize = sizeof(aiVector3D);
					break;
				default:
					MH_CORE_ASSERT(false, "Unknown shader semantic!");
					break;
				}

				for (unsigned int i = 0; i < mesh->mNumVertices; i++)
				{
					uint32_t srcOffset = i * srcSize;
					uint32_t dstOffset = i * size;
					memcpy(vertex.data + dstOffset, srcArray + srcOffset, size);
				}

				vertices.push_back(vertex);
			}

			uint32_t indexOffset = 0;
			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				memcpy(indices + indexOffset, face.mIndices, sizeof(uint32_t) * face.mNumIndices);

				indexOffset += face.mNumIndices;
			}

			Ref<Mesh> m = Mesh::create(vertexCount, meshLayout.getBufferLayout(), indices, indexCount);
			for (auto& vert : vertices)
				m->addVertices(vert.name, vert.data);
			m->init();

			for (auto& vert : vertices)
				delete[] vert.data;

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
		Model(const std::initializer_list<Ref<Mesh>>& meshes, const MeshLayout& layout)
			: rootBone(0), meshes(meshes), meshLayout(layout) {}

		Model(const std::initializer_list<Ref<Mesh>>& meshes, ModelBone* bone, const MeshLayout& layout)
			: rootBone(bone), meshes(meshes), meshLayout(layout) {}

		Model(const std::string& filepath, const MeshLayout& layout)
			: rootBone(0), meshLayout(layout)
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

		static Ref<Model> load(const std::string& filepath, const MeshLayout& meshLayout)
		{
			return std::make_shared<Model>(filepath, meshLayout);
		}
	};
}