#include "Mahakam/mhpch.h"
#include "Mesh.h"

#include "RendererAPI.h"
#include "Material.h"

#include "Mahakam/Core/Profiler.h"
#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/OpenGL/OpenGLMesh.h"

#include <tiny_gltf/tiny_gltf.h>

namespace Mahakam
{
	static glm::vec3 CalculateCubeSphereVertex(const glm::vec3& v)
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

	static glm::vec2 CalculateEquirectangularUVs(const glm::vec3& v)
	{
		const glm::vec2 invAtan = glm::vec2(0.1591, 0.3183);
		glm::vec2 uv = glm::vec2(glm::atan(v.z, v.x), glm::asin(v.y));
		uv *= invAtan;
		uv += 0.5;
		return uv;
	}

	static glm::vec3 CalculateUVSphere(const glm::vec2& coords)
	{
		const float pi = glm::pi<float>();
		const float pi2 = pi * 2.0f;

		float theta = coords.x * pi2;
		float phi = (coords.y - 0.5f) * pi;

		while (theta >= pi2)
			theta -= pi2;

		// This determines the radius of the ring of this line of latitude.
		// It's widest at the equator, and narrows as phi increases/decreases.
		float c = glm::cos(phi);

		// Usual formula for a vector in spherical coordinates.
		// You can exchange x & z to wind the opposite way around the sphere.
		return glm::normalize(glm::vec3(
			c * glm::cos(theta),
			glm::sin(phi),
			c * glm::sin(theta)
		));
	}

	template<typename T>
	void GLTFLoadAttribute(tinygltf::Model& model, tinygltf::Primitive& p, const std::string& attribute, size_t& offset, T* dst)
	{
		auto iter = p.attributes.find(attribute);
		if (iter == p.attributes.end()) return;

		const auto& accessor = model.accessors[iter->second];
		const auto& bufferView = model.bufferViews[accessor.bufferView];
		const auto& buffer = model.buffers[bufferView.buffer];

		const void* bufferData = &buffer.data[bufferView.byteOffset + accessor.byteOffset];

		size_t componentCount = tinygltf::GetNumComponentsInType(accessor.type);
		size_t componentSize = tinygltf::GetComponentSizeInBytes(accessor.componentType);

		if (componentSize * componentCount > sizeof(T))
		{
			MH_CORE_BREAK("[GLTF] Downscaling attributes is not currently supported");
		}
		else if (componentSize * componentCount < sizeof(T)) // Ex: the file uses 8 bits, but the buffer we allocate uses 32 bits
		{
			// Assume that the type is the same for both the file and buffer (eg. vec3)
			size_t outComponentSize = sizeof(T) / componentCount;

			// Copy each component of the type individually
			memset(dst + offset, 0, accessor.count * sizeof(T));
			for (size_t i = 0; i < accessor.count * componentCount; i++)
				memcpy(((uint8_t*)dst) + offset * sizeof(T) + i * outComponentSize, ((uint8_t*)bufferData) + i * componentSize, componentSize);
		}
		else // Copy straight, no mismatch. Type should be interpreted, so it doesn't matter
		{
			memcpy(dst + offset, bufferData, accessor.count * sizeof(T));
		}

		offset += accessor.count;
	}

	void Mesh::GLTFReadNodeHierarchy(const tinygltf::Model& model, int id, int parentID, Ref<Mesh> skinnedMesh)
	{
		const tinygltf::Node& node = model.nodes[id];

		BoneNode bone;
		bone.name = node.name;
		bone.id = id;
		bone.parentID = parentID;
		skinnedMesh->BoneHierarchy.push_back(bone);

		for (auto& child : node.children)
			GLTFReadNodeHierarchy(model, child, id, skinnedMesh);
	}

	//Asset<Mesh> Mesh::LoadMeshImpl(const std::filesystem::path& filepath, const MeshProps& props)
	MH_DEFINE_FUNC(Mesh::LoadMeshImpl, Asset<Mesh>, const std::filesystem::path& filepath, const MeshProps& props)
	{
		MH_PROFILE_FUNCTION();

		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		bool success;
		if (filepath.extension().string() == ".gltf")
			success = loader.LoadASCIIFromFile(&model, &err, &warn, filepath.string());
		else
			success = loader.LoadBinaryFromFile(&model, &err, &warn, filepath.string());

		if (!warn.empty())
			MH_CORE_WARN("[GLTF] Warning: {0}", warn);

		if (!err.empty())
			MH_CORE_ERROR("[GLTF] Error: {0}", err);

		if (!success) {
			MH_CORE_ERROR("[GLTF] Failed to parse glTF model at {0}", filepath.string());
			return nullptr;
		}

		MH_CORE_ASSERT(!model.scenes.empty(), "[GLTF] Model scenes are empty!");

		auto& scene = model.scenes[model.defaultScene];

		MH_CORE_ASSERT(!scene.nodes.empty(), "[GLTF] Model nodes are empty!");


		Ref<Mesh> skinnedMesh = CreateRef<Mesh>();

		// Extract vertex and index values
		for (auto& m : model.meshes)
		{
			// Calculate index and vertex count
			uint32_t indexCount = 0;
			uint32_t vertexCount = 0;
			for (auto& p : m.primitives)
			{
				const auto& posAccessor = model.accessors[p.attributes["POSITION"]];
				const auto& indAccessor = model.accessors[p.indices];

				vertexCount += (uint32_t)posAccessor.count;
				indexCount += (uint32_t)indAccessor.count;
			}

			// Setup variables
			glm::vec3* positions = new glm::vec3[vertexCount];
			glm::vec2* texcoords = new glm::vec2[vertexCount];
			glm::vec3* normals = new glm::vec3[vertexCount];
			glm::vec4* tangents = new glm::vec4[vertexCount];
			glm::vec4* colors = new glm::vec4[vertexCount];
			glm::ivec4* boneIDs = new glm::ivec4[vertexCount];
			glm::vec4* boneWeights = new glm::vec4[vertexCount];
			uint32_t* indices = new uint32_t[indexCount]{ 0 };

			size_t positionOffset = 0;
			size_t texcoordOffset = 0;
			size_t normalOffset = 0;
			size_t tangentOffset = 0;
			size_t colorOffset = 0;
			size_t boneIDOffset = 0;
			size_t boneWeightOffset = 0;
			size_t indexOffset = 0;

			for (uint32_t i = 0; i < vertexCount; i++)
			{
				boneIDs[i] = glm::vec4(-1.0, -1.0, -1.0, -1.0);
				boneWeights[i] = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			}

			// Load attributes and indices
			for (auto& p : m.primitives)
			{
				// Extract positions
				GLTFLoadAttribute<glm::vec3>(model, p, "POSITION", positionOffset, positions);

				// Extract texcoords
				GLTFLoadAttribute<glm::vec2>(model, p, "TEXCOORD_0", texcoordOffset, texcoords);

				// Extract normals
				GLTFLoadAttribute<glm::vec3>(model, p, "NORMAL", normalOffset, normals);

				// Extract tangents
				GLTFLoadAttribute<glm::vec4>(model, p, "TANGENT", tangentOffset, tangents);

				// Extract vertex colors
				GLTFLoadAttribute<glm::vec4>(model, p, "COLOR_0", colorOffset, colors);

				// Extract joint information
				GLTFLoadAttribute<glm::ivec4>(model, p, "JOINTS_0", boneIDOffset, boneIDs);

				// Extract weight information
				GLTFLoadAttribute<glm::vec4>(model, p, "WEIGHTS_0", boneWeightOffset, boneWeights);

				// Normalize weights
				{
					/*for (size_t i = 0; i < boneWeightOffset; i++)
					{
						glm::vec4& weight = boneWeights[i];

						float sum = weight.x + weight.y + weight.z + weight.w;
					}*/
				}

				// Extract indices
				{
					const auto& accessor = model.accessors[p.indices];
					const auto& bufferView = model.bufferViews[accessor.bufferView];
					const auto& buffer = model.buffers[bufferView.buffer];

					const uint8_t* indexData = &buffer.data[bufferView.byteOffset + accessor.byteOffset];

					switch (accessor.componentType)
					{
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
						MH_CORE_INFO("[GLTF] Loading indices with uint32_t");
						memcpy(indices + indexOffset, indexData, accessor.count * sizeof(uint32_t));
						break;
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
						MH_CORE_INFO("[GLTF] Loading indices with uint16_t");
						for (size_t i = 0; i < accessor.count; i++)
							memcpy(indices + i + indexOffset, indexData + i * sizeof(uint16_t), sizeof(uint16_t));
						break;
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
						MH_CORE_INFO("[GLTF] Loading indices with uint8_t");
						for (size_t i = 0; i < accessor.count; i++)
							memcpy(indices + i + indexOffset, indexData + i * sizeof(uint8_t), sizeof(uint8_t));
						break;
					default:
						MH_CORE_BREAK("[GLTF] Unsupported index format");
						break;
					}

					indexOffset += accessor.count;
				}
			}

			// Extract bone transformations
			for (auto& skinNode : model.nodes)
			{
				if (skinNode.skin > -1)
				{
					// Get the skin from the node
					auto& skin = model.skins[skinNode.skin];

					// TODO: The spec says to use it, but the results are weird
					// Currently works better without, but be wary
					auto& skinPos = skinNode.translation;
					auto& skinRot = skinNode.rotation;
					auto& skinScale = skinNode.scale;

					glm::mat4 skinnedTransform{ 1.0f };
					if (!skinPos.empty())
						skinnedTransform *= glm::translate(glm::mat4(1.0f), glm::vec3{ (float)skinPos[0], (float)skinPos[1], (float)skinPos[2] });
					if (!skinRot.empty())
						skinnedTransform *= glm::toMat4(glm::quat{ (float)skinRot[0], (float)skinRot[1], (float)skinRot[2], (float)skinRot[3] });
					if (!skinScale.empty())
						skinnedTransform *= glm::scale(glm::mat4(1.0f), glm::vec3{ (float)skinScale[0], (float)skinScale[1], (float)skinScale[2] });

					glm::mat4 invSkinnedTransform = glm::inverse(skinnedTransform);

					// Get the affected nodes and their bind matrices
					auto& joints = skin.joints;
					const auto& accessor = model.accessors[skin.inverseBindMatrices];
					const auto& bufferView = model.bufferViews[accessor.bufferView];
					const auto& buffer = model.buffers[bufferView.buffer];

					const glm::mat4* invMatrices = reinterpret_cast<const glm::mat4*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

					// Extract hierarchy data
					skinnedMesh->BoneCount = joints.size();
					skinnedMesh->BoneHierarchy.reserve(joints.size());
					GLTFReadNodeHierarchy(model, joints[0], -1, skinnedMesh);

					MH_CORE_ASSERT(skinnedMesh->BoneHierarchy.size() == skinnedMesh->BoneCount, "Bone hierarchy count doesn't match skeleton");

					// Add the bones to the skinned mesh
					for (uint32_t i = 0; i < joints.size(); i++)
					{
						const auto& node = model.nodes[joints[i]];

						std::string boneName = node.name;

						auto& nodePos = node.translation;
						auto& nodeRot = node.rotation;
						auto& nodeScale = node.scale;

						glm::mat4 jointTransform{ 1.0f };
						if (!nodePos.empty())
							jointTransform *= glm::translate(glm::mat4(1.0f), glm::vec3{ (float)nodePos[0], (float)nodePos[1], (float)nodePos[2] });
						if (!nodeRot.empty())
							jointTransform *= glm::toMat4(glm::quat{ (float)nodeRot[0], (float)nodeRot[1], (float)nodeRot[2], (float)nodeRot[3] });
						if (!nodeScale.empty())
							jointTransform *= glm::scale(glm::mat4(1.0f), glm::vec3{ (float)nodeScale[0], (float)nodeScale[1], (float)nodeScale[2] });

						BoneInfo bone;
						bone.id = i;
						bone.offset = /*invSkinnedTransform * jointTransform */ invMatrices[i];
						skinnedMesh->BoneInfoMap[boneName] = bone;
					}
				}
			}

			MH_CORE_ASSERT(vertexCount == positionOffset, "Vertex count mismatch");
			MH_CORE_ASSERT(indexCount == indexOffset, "Index count mismatch");

			// Interleave vertices
			SubMesh::InterleavedStruct interleavedVertices;

			if (positionOffset)
				interleavedVertices.positions = positions;
			if (texcoordOffset)
				interleavedVertices.texcoords = texcoords;
			if (normalOffset)
				interleavedVertices.normals = normals;
			if (tangentOffset)
				interleavedVertices.tangents = tangents;
			if (colorOffset)
				interleavedVertices.colors = colors;
			if (boneIDOffset && boneWeightOffset)
			{
				interleavedVertices.boneIDs = boneIDs;
				interleavedVertices.boneWeights = boneWeights;
			}

			Asset<SubMesh> mesh = SubMesh::Create(vertexCount, indexCount, interleavedVertices, indices);

			delete[] positions;
			delete[] texcoords;
			delete[] normals;
			delete[] tangents;
			delete[] colors;
			delete[] boneIDs;
			delete[] boneWeights;
			delete[] indices;

			skinnedMesh->Meshes.push_back(mesh);
		}

		return Asset<Mesh>(skinnedMesh);
	};

	//Ref<Mesh> Mesh::CreateImpl(uint32_t vertexCount, uint32_t indexCount, const void* verts[BUFFER_ELEMENTS_SIZE], const uint32_t* indices)
	MH_DEFINE_FUNC(SubMesh::CreateImpl, Asset<SubMesh>, uint32_t vertexCount, uint32_t indexCount, const void* verts[BUFFER_ELEMENTS_SIZE], const uint32_t* indices)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			MH_CORE_BREAK("Renderer API not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return Asset<OpenGLMesh>(CreateRef<OpenGLMesh>(vertexCount, indexCount, verts, indices));
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};

	Asset<SubMesh> SubMesh::CreateCube(int tessellation, bool reverse)
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

		// Interleave vertices
		InterleavedStruct interleavedVertices;
		interleavedVertices.positions = positions;
		interleavedVertices.texcoords = uvs;
		interleavedVertices.normals = normals;

		Asset<SubMesh> mesh = SubMesh::Create(vertexCount, indexCount, interleavedVertices, indices);

		delete[] positions;
		delete[] uvs;
		delete[] normals;
		delete[] indices;

		return mesh;
	}

	Asset<SubMesh> SubMesh::CreatePlane(int rows, int columns)
	{
		MH_PROFILE_FUNCTION();

		uint32_t vertexCount = rows * columns;
		uint32_t indexCount = 6 * (rows - 1) * (columns - 1);

		glm::vec3* positions = new glm::vec3[vertexCount];
		glm::vec2* uvs = new glm::vec2[vertexCount];
		glm::vec3* normals = new glm::vec3[vertexCount];
		glm::vec4* tangents = new glm::vec4[vertexCount];

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
				tangents[index] = { -1.0f, 0.0f, 0.0f, 1.0f }; // Positive or negative?

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

		// Interleave vertices
		InterleavedStruct interleavedVertices;
		interleavedVertices.positions = positions;
		interleavedVertices.texcoords = uvs;
		interleavedVertices.normals = normals;
		interleavedVertices.tangents = tangents;

		Asset<SubMesh> mesh = SubMesh::Create(vertexCount, indexCount, interleavedVertices, indices);

		delete[] positions;
		delete[] uvs;
		delete[] normals;
		delete[] tangents;
		delete[] indices;

		return mesh;
	}

	Asset<SubMesh> SubMesh::CreateUVSphere(int rows, int columns)
	{
		MH_PROFILE_FUNCTION();

		uint32_t vertexCount = rows * columns;
		uint32_t indexCount = 6 * (rows - 1) * (columns - 1);

		glm::vec3* positions = new glm::vec3[vertexCount];
		glm::vec2* uvs = new glm::vec2[vertexCount];
		glm::vec3* normals = new glm::vec3[vertexCount];
		glm::vec4* tangents = new glm::vec4[vertexCount];

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
				glm::vec2 percentCenter = { x / (float)(rows - 1), y / (float)(columns - 1) };
				glm::vec2 percentRight = { (x + 1) / (float)(rows - 1), y / (float)(columns - 1) };

				glm::vec3 pointOnSphereCenter = CalculateUVSphere(percentCenter);
				glm::vec3 pointOnSphereRight = CalculateUVSphere(percentRight);

				positions[index] = pointOnSphereCenter * 0.5f;
				uvs[index] = { percentCenter.x, 1.0f - percentCenter.y };
				normals[index] = pointOnSphereCenter;
				tangents[index] = { glm::normalize(pointOnSphereRight - pointOnSphereCenter), 1.0f };

				if (x != rows - 1 && y != columns - 1)
				{
					indices[triIndex] = index;
					indices[triIndex + 1] = index + rows;
					indices[triIndex + 2] = index + rows + 1;

					indices[triIndex + 3] = index;
					indices[triIndex + 4] = index + rows + 1;
					indices[triIndex + 5] = index + 1;

					triIndex += 6;
				}

				index++;
			}
		}

		// Interleave vertices
		InterleavedStruct interleavedVertices;
		interleavedVertices.positions = positions;
		interleavedVertices.texcoords = uvs;
		interleavedVertices.normals = normals;
		interleavedVertices.tangents = tangents;

		Asset<SubMesh> mesh = SubMesh::Create(vertexCount, indexCount, interleavedVertices, indices);

		delete[] positions;
		delete[] uvs;
		delete[] normals;
		delete[] tangents;
		delete[] indices;

		return mesh;
	}

	Asset<SubMesh> SubMesh::CreateCubeSphere(int tessellation, bool reverse, bool equirectangular)
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
		glm::vec4* tangents = new glm::vec4[vertexCount];

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

					glm::vec3 pointOnCubeRight = upwards
						+ (percent.x + 1 / (float)(tessellation - 1) - 0.5f) * 2.0f * axisA
						+ (percent.y - 0.5f) * 2.0f * axisB;

					glm::vec3 pointOnSphere = CalculateCubeSphereVertex(pointOnCube);
					glm::vec3 pointOnSphereRight = CalculateCubeSphereVertex(pointOnCubeRight);

					positions[index] = pointOnSphere * 0.5f;
					if (equirectangular)
						uvs[index] = CalculateEquirectangularUVs(pointOnSphere);
					else
						uvs[index] = percent;
					normals[index] = pointOnSphere;
					tangents[index] = { glm::normalize(pointOnSphereRight - pointOnSphere), 1.0f };

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

		// Interleave vertices
		InterleavedStruct interleavedVertices;
		interleavedVertices.positions = positions;
		interleavedVertices.texcoords = uvs;
		interleavedVertices.normals = normals;
		interleavedVertices.tangents = tangents;

		Asset<SubMesh> mesh = SubMesh::Create(vertexCount, indexCount, interleavedVertices, indices);

		delete[] positions;
		delete[] uvs;
		delete[] normals;
		delete[] tangents;
		delete[] indices;

		return mesh;
	}
}

template class Mahakam::Asset<Mahakam::Mesh>;
template class Mahakam::Asset<Mahakam::SubMesh>;