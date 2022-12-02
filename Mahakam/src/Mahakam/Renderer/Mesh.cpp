#include "Mahakam/mhpch.h"
#include "Mesh.h"

#include "RendererAPI.h"
#include "Material.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Profiler.h"
#include "Mahakam/Core/SharedLibrary.h"

#include "Platform/Headless/HeadlessMesh.h"
#include "Platform/OpenGL/OpenGLMesh.h"

#include <glm/gtx/fast_square_root.hpp>

#include <tiny_gltf/tiny_gltf.h>

namespace Mahakam
{
	static glm::vec3 CalculateCubeSphereVertex(const glm::vec3& v)
	{
		float x2 = v.x * v.x;
		float y2 = v.y * v.y;
		float z2 = v.z * v.z;
		glm::vec3 s;
		s.x = v.x * glm::fastSqrt(1.0f - y2 / 2.0f - z2 / 2.0f + y2 * z2 / 3.0f);
		s.y = v.y * glm::fastSqrt(1.0f - x2 / 2.0f - z2 / 2.0f + x2 * z2 / 3.0f);
		s.z = v.z * glm::fastSqrt(1.0f - x2 / 2.0f - y2 / 2.0f + x2 * y2 / 3.0f);
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
		return glm::fastNormalize(glm::vec3(
			c * glm::cos(theta),
			glm::sin(phi),
			c * glm::sin(theta)
		));
	}

	template<typename T>
	void GLTFLoadAttribute(tinygltf::Model& model, tinygltf::Primitive& p, const std::string& attribute, size_t& offset, TrivialArray<T>& dst)
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
			memset(dst.data() + offset, 0, accessor.count * sizeof(T));
			for (size_t i = 0; i < accessor.count * componentCount; i++)
				memcpy(reinterpret_cast<uint8_t*>(dst.data()) + offset * sizeof(T) + i * outComponentSize, ((uint8_t*)bufferData) + i * componentSize, componentSize);
		}
		else // Copy straight, no mismatch. Type should be interpreted, so it doesn't matter
		{
			memcpy(dst.data() + offset, bufferData, accessor.count * sizeof(T));
		}

		offset += accessor.count;
	}

	void Mesh::GLTFReadNodeHierarchy(const tinygltf::Model& model, UnorderedMap<int, size_t>& nodeIndex, int id, int parentID, Mesh* skinnedMesh)
	{
		const tinygltf::Node& node = model.nodes[id];

		MeshNode bone;
		bone.name = node.name;
		bone.id = id;
		bone.mesh = node.mesh;
		bone.parentID = parentID;

		if (node.matrix.size() > 0)
		{
			auto& m = node.matrix;
			glm::mat4 matrix = glm::mat4{
				m[0], m[1], m[2], m[3],
				m[4], m[5], m[6], m[7],
				m[8], m[9], m[10], m[11],
				m[12], m[13], m[14], m[15] };
			bone.offset = glm::inverse(matrix);
		}
		else
		{
			auto& nodePos = node.translation;
			auto& nodeRot = node.rotation;
			auto& nodeScale = node.scale;

			glm::mat4 matrix{ 1.0f };
			if (!nodePos.empty())
				matrix *= glm::translate(glm::mat4(1.0f), glm::vec3{ (float)nodePos[0], (float)nodePos[1], (float)nodePos[2] });
			if (!nodeRot.empty())
				matrix *= glm::toMat4(glm::quat{ (float)nodeRot[0], (float)nodeRot[1], (float)nodeRot[2], (float)nodeRot[3] });
			if (!nodeScale.empty())
				matrix *= glm::scale(glm::mat4(1.0f), glm::vec3{ (float)nodeScale[0], (float)nodeScale[1], (float)nodeScale[2] });

			bone.offset = glm::inverse(matrix);
		}

		nodeIndex[id] = skinnedMesh->NodeHierarchy.size();
		skinnedMesh->NodeHierarchy.push_back(bone);

		for (auto& child : node.children)
			GLTFReadNodeHierarchy(model, nodeIndex, child, id, skinnedMesh);
	}

	//Asset<Mesh> Mesh::CreatePropsImpl(const MeshProps& props)
	MH_DEFINE_FUNC(Mesh::CreatePropsImpl, Asset<Mesh>, const MeshProps& props)
	{
		return CreateAsset<Mesh>(props);
	};

	//Asset<Mesh> Mesh::CreateSubMeshImpl(Ref<SubMesh> subMesh, const MeshProps& props)
	MH_DEFINE_FUNC(Mesh::CreateSubMeshImpl, Asset<Mesh>, Ref<SubMesh> subMesh, const MeshProps& props)
	{
		return CreateAsset<Mesh>(subMesh, props);
	};

	//Asset<Mesh> Mesh::CreatePropsImpl(const MeshProps& props)
	MH_DEFINE_FUNC(Mesh::CopyImpl, Asset<Mesh>, Asset<Mesh> other)
	{
		return CreateAsset<Mesh>(*other.get());
	};

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

		if (model.scenes.size() > 1)
			MH_CORE_WARN("[GLTF] Only 1 scene is supported. Other scenes will be ignored");

		MH_CORE_ASSERT(!scene.nodes.empty(), "[GLTF] Model nodes are empty!");


		// TODO: Support interleaved data
		// TODO: Support sparse data sets


		Mesh* skinnedMesh = Allocator::Allocate<Mesh>(1);
		Allocator::Construct<Mesh>(skinnedMesh);

		auto deleter = [](void* p)
		{
			Allocator::Deconstruct<Mesh>(static_cast<Mesh*>(p));
			Allocator::Deallocate<Mesh>(static_cast<Mesh*>(p), 1);
		};

		skinnedMesh->Props = props;

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
			TrivialArray<glm::vec3> positions(vertexCount);
			TrivialArray<glm::vec2> texcoords(vertexCount);
			TrivialArray<glm::vec3> normals(vertexCount);
			TrivialArray<glm::vec4> tangents(vertexCount);
			TrivialArray<glm::vec4> colors(vertexCount);
			TrivialArray<glm::ivec4> boneIDs(vertexCount, { -1, -1, -1, -1 });
			TrivialArray<glm::vec4> boneWeights(vertexCount, { 0.0f, 0.0f, 0.0f, 0.0f });
			TrivialArray<uint32_t> indices(indexCount, 0);

			size_t positionOffset = 0;
			size_t texcoordOffset = 0;
			size_t normalOffset = 0;
			size_t tangentOffset = 0;
			size_t colorOffset = 0;
			size_t boneIDOffset = 0;
			size_t boneWeightOffset = 0;
			size_t indexOffset = 0;

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

				if (props.IncludeBones)
				{
					// Extract joint information
					GLTFLoadAttribute<glm::ivec4>(model, p, "JOINTS_0", boneIDOffset, boneIDs);

					// Extract weight information
					GLTFLoadAttribute<glm::vec4>(model, p, "WEIGHTS_0", boneWeightOffset, boneWeights);
				}

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
					const auto byteStride = accessor.ByteStride(bufferView);

					const uint8_t* indexData = &buffer.data[bufferView.byteOffset + accessor.byteOffset];

					switch (accessor.componentType)
					{
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
						MH_CORE_INFO("[GLTF] Loading indices with uint32_t");
						memcpy(indices.data() + indexOffset, indexData, accessor.count * sizeof(uint32_t));
						break;
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
						MH_CORE_INFO("[GLTF] Loading indices with uint16_t");
						for (size_t i = 0; i < accessor.count; i++)
							memcpy(indices.data() + i + indexOffset, indexData + i * sizeof(uint16_t), sizeof(uint16_t));
						break;
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
						MH_CORE_INFO("[GLTF] Loading indices with uint8_t");
						for (size_t i = 0; i < accessor.count; i++)
							memcpy(indices.data() + i + indexOffset, indexData + i * sizeof(uint8_t), sizeof(uint8_t));
						break;
					default:
						MH_CORE_BREAK("[GLTF] Unsupported index format");
						break;
					}

					indexOffset += accessor.count;
				}
			}

			MH_CORE_ASSERT(vertexCount == positionOffset, "Vertex count mismatch");
			MH_CORE_ASSERT(indexCount == indexOffset, "Index count mismatch");

			// Interleave vertices
			SubMesh::InterleavedStruct interleavedVertices;

			if (positionOffset)
				interleavedVertices.positions = positions.data();
			if (texcoordOffset)
				interleavedVertices.texcoords = texcoords.data();
			if (normalOffset)
				interleavedVertices.normals = normals.data();
			if (tangentOffset)
				interleavedVertices.tangents = tangents.data();
			if (colorOffset)
				interleavedVertices.colors = colors.data();
			if (boneIDOffset && boneWeightOffset)
			{
				interleavedVertices.boneIDs = boneIDs.data();
				interleavedVertices.boneWeights = boneWeights.data();
			}

			Ref<SubMesh> mesh = SubMesh::Create(vertexCount, indexCount, interleavedVertices, indices.data());

			skinnedMesh->Meshes.push_back(mesh);
		}

		// Extract nodes and bones
		if (props.IncludeNodes)
		{
			for (int rootNode : scene.nodes)
			{
				UnorderedMap<int, size_t> nodeIndex; // Node ID to hierarchy index

				// Populate node hierarchy
				GLTFReadNodeHierarchy(model, nodeIndex, rootNode, -1, skinnedMesh);

				// Extract bone transformations
				if (props.IncludeBones)
				{
					for (auto& skinNode : model.nodes)
					{
						if (skinNode.skin > -1)
						{
							// Get the skin from the node
							auto& skin = model.skins[skinNode.skin];

							// Get the affected nodes and their bind matrices
							auto& joints = skin.joints;
							const auto& accessor = model.accessors[skin.inverseBindMatrices];
							const auto& bufferView = model.bufferViews[accessor.bufferView];
							const auto& buffer = model.buffers[bufferView.buffer];

							MH_CORE_ASSERT(joints.size() == accessor.count, "Bone count doesn't match joint count");

							const glm::mat4* invMatrices = reinterpret_cast<const glm::mat4*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

							// Extract joint data
							skinnedMesh->BoneInfoMap.reserve(skinnedMesh->BoneInfoMap.size() + joints.size());

							// Add the bones to the skinned mesh
							for (size_t i = 0; i < joints.size(); i++)
							{
								int nodeID = joints[i];
								const auto& node = model.nodes[nodeID];

								// TODO: The spec is vague on when to do 'inv(parentMat) * nodeMat * invBindMatrix'
								// For now it is done inside the Scene, once per frame
								skinnedMesh->NodeHierarchy[nodeIndex[nodeID]].offset = invMatrices[i]; // Override offset to be the bone's inverse matrix
								skinnedMesh->BoneInfoMap.insert(node.name, (int)i);
							}
						}
					}
				}
			}

			MH_CORE_ASSERT(skinnedMesh->NodeHierarchy.size() == model.nodes.size(), "Node hierarchy doesn't match model");
		}

		return Asset<Mesh>(skinnedMesh, deleter);
	};

	//Ref<SubMesh> Mesh::CreateImpl(uint32_t vertexCount, uint32_t indexCount, const void* verts[BUFFER_ELEMENTS_SIZE], const uint32_t* indices)
	MH_DEFINE_FUNC(SubMesh::CreateImpl, Ref<SubMesh>, uint32_t vertexCount, uint32_t indexCount, const void* verts[BUFFER_ELEMENTS_SIZE], const uint32_t* indices)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			return CreateRef<HeadlessMesh>(vertexCount, indexCount, verts, indices);
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLMesh>(vertexCount, indexCount, verts, indices);
		}

		MH_CORE_BREAK("Unknown renderer API!");

		return nullptr;
	};

	Ref<SubMesh> SubMesh::CreateCube(int tessellation, bool reverse)
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

		TrivialArray<glm::vec3> positions(vertexCount);
		TrivialArray<glm::vec2> uvs(vertexCount);
		TrivialArray<glm::vec3> normals(vertexCount);
		TrivialArray<glm::vec4> tangents(vertexCount);
		TrivialArray<uint32_t> indices(indexCount);

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
		interleavedVertices.positions = positions.data();
		interleavedVertices.texcoords = uvs.data();
		interleavedVertices.normals = normals.data();
		//interleavedVertices.tangents = tangents.data(); // TODO

		Ref<SubMesh> mesh = SubMesh::Create(vertexCount, indexCount, interleavedVertices, indices.data());

		return mesh;
	}

	Ref<SubMesh> SubMesh::CreatePlane(int rows, int columns)
	{
		MH_PROFILE_FUNCTION();

		uint32_t vertexCount = rows * columns;
		uint32_t indexCount = 6 * (rows - 1) * (columns - 1);

		TrivialArray<glm::vec3> positions(vertexCount);
		TrivialArray<glm::vec2> uvs(vertexCount);
		TrivialArray<glm::vec3> normals(vertexCount);
		TrivialArray<glm::vec4> tangents(vertexCount);
		TrivialArray<uint32_t> indices(indexCount);

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
		interleavedVertices.positions = positions.data();
		interleavedVertices.texcoords = uvs.data();
		interleavedVertices.normals = normals.data();
		interleavedVertices.tangents = tangents.data();

		Ref<SubMesh> mesh = SubMesh::Create(vertexCount, indexCount, interleavedVertices, indices.data());

		return mesh;
	}

	Ref<SubMesh> SubMesh::CreateUVSphere(int rows, int columns)
	{
		MH_PROFILE_FUNCTION();

		uint32_t vertexCount = rows * columns;
		uint32_t indexCount = 6 * (rows - 1) * (columns - 1);

		TrivialArray<glm::vec3> positions(vertexCount);
		TrivialArray<glm::vec2> uvs(vertexCount);
		TrivialArray<glm::vec3> normals(vertexCount);
		TrivialArray<glm::vec4> tangents(vertexCount);
		TrivialArray<uint32_t> indices(indexCount);

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
				tangents[index] = { glm::fastNormalize(pointOnSphereRight - pointOnSphereCenter), 1.0f };

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
		interleavedVertices.positions = positions.data();
		interleavedVertices.texcoords = uvs.data();
		interleavedVertices.normals = normals.data();
		interleavedVertices.tangents = tangents.data();

		Ref<SubMesh> mesh = SubMesh::Create(vertexCount, indexCount, interleavedVertices, indices.data());

		return mesh;
	}

	Ref<SubMesh> SubMesh::CreateCubeSphere(int tessellation, bool reverse, bool equirectangular)
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

		TrivialArray<glm::vec3> positions(vertexCount);
		TrivialArray<glm::vec2> uvs(vertexCount);
		TrivialArray<glm::vec3> normals(vertexCount);
		TrivialArray<glm::vec4> tangents(vertexCount);
		TrivialArray<uint32_t> indices(indexCount);

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
					tangents[index] = { glm::fastNormalize(pointOnSphereRight - pointOnSphere), 1.0f };

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
		interleavedVertices.positions = positions.data();
		interleavedVertices.texcoords = uvs.data();
		interleavedVertices.normals = normals.data();
		interleavedVertices.tangents = tangents.data();

		Ref<SubMesh> mesh = SubMesh::Create(vertexCount, indexCount, interleavedVertices, indices.data());

		return mesh;
	}
}