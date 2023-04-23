#include "Mahakam/mhpch.h"
#include "SceneDef.h"

#include "Components/AnimatorComponent.h"
#include "Components/AudioListenerComponent.h"
#include "Components/AudioSourceComponent.h"
#include "Components/CameraComponent.h"
#include "Components/FlagComponents.h"
#include "Components/LightComponent.h"
#include "Components/MeshComponent.h"
#include "Components/ParticleSystemComponent.h"
#include "Components/RelationshipComponent.h"
#include "Components/SkeletonComponent.h"
#include "Components/TagComponent.h"
#include "Components/TransformComponent.h"
#include "Entity.h"

#include "Mahakam/Asset/AssetDatabase.h"

#include "Mahakam/Audio/AudioEngine.h"

#include "Mahakam/Core/FileUtility.h"

#include "Mahakam/Math/Math.h"

#include "Mahakam/Physics/PhysicsContext.h"

#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/RenderData.h"
#include "Mahakam/Renderer/Renderer.h"
#include "Mahakam/Renderer/Shader.h"
#include "Mahakam/Renderer/Texture.h"

#include <fstream>

namespace Mahakam
{
	//static Asset<TextureCube> LoadOrCreate(const std::string& cachePath, Ref<TextureCube> src, bool saveMips, TextureCubePrefilter prefilter, const CubeTextureProps& props)
	//{
	//	if (!FileUtility::Exists(cachePath))
	//	{
	//		Asset<TextureCube> texture = TextureCube::Create(src, prefilter, props);

	//		uint32_t size = texture->GetSize();
	//		uint32_t totalSize = texture->GetTotalSize();

	//		// Save to cache
	//		char* pixels = new char[totalSize];
	//		texture->ReadPixels(pixels, saveMips);
	//		std::ofstream stream(cachePath, std::ios::binary);
	//		stream.write((char*)&size, sizeof(uint32_t));
	//		stream.write(pixels, totalSize);

	//		delete[] pixels;

	//		return texture;
	//	}
	//	else
	//	{
	//		// Load from cache
	//		std::ifstream stream(cachePath, std::ios::binary);
	//		std::stringstream ss;
	//		uint32_t size = 0;
	//		stream.read((char*)&size, sizeof(uint32_t));
	//		ss << stream.rdbuf();
	//		Asset<TextureCube> texture = TextureCube::Create(props);
	//		texture->SetData((void*)ss.str().c_str(), size, saveMips);

	//		stream.close();

	//		return texture;
	//	}
	//}



	static size_t GetDepth(entt::registry& registry, entt::entity parent)
	{
		size_t depth = 0;
		while (parent != entt::null)
		{
			RelationshipComponent& relation = registry.get<RelationshipComponent>(parent);
			parent = relation.Parent;
			depth++;
		}

		return depth;
	}

	Scene::Scene(PhysicsContext* physics) :
		m_PhysicsContext(physics)
	{
		MH_PROFILE_FUNCTION();

		Asset<Shader> skyboxShader = Shader::Create("assets/shaders/default/Skybox.shader");
		m_Environment.SkyboxMaterial = Material::Create(skyboxShader);
		m_Environment.SkyboxMaterial->SetTexture("u_Environment", 0, GL::GetTextureCubeWhite());
	}

	Scene::Scene(const std::string& filepath, PhysicsContext* physics) :
		m_PhysicsContext(physics)
	{
		MH_PROFILE_FUNCTION();

		Asset<Texture> skyboxTexture = TextureCube::Create(filepath, { 4096, TextureFormat::RG11B10F });
		m_Environment.IrradianceMap = TextureCube::Create(filepath, { 64, TextureFormat::RG11B10F, TextureCubePrefilter::Convolute, false });
		m_Environment.SpecularMap = TextureCube::Create(filepath, { 512, TextureFormat::RG11B10F, TextureCubePrefilter::Prefilter, true });

		//skyboxIrradiance = AssetDatabase::CreateOrLoadAsset<TextureCube>(filepath + ".irradiance", skyboxTexture, false, TextureCubePrefilter::Convolute, { 64, TextureFormat::RG11B10F, false });
		//skyboxSpecular = AssetDatabase::CreateOrLoadAsset<TextureCube>(filepath + ".specular", skyboxTexture, true, TextureCubePrefilter::Prefilter, { 512, TextureFormat::RG11B10F, true });

		Asset<Shader> skyboxShader = Shader::Create("assets/shaders/default/Skybox.shader");
		m_Environment.SkyboxMaterial = Material::Create(skyboxShader);
		m_Environment.SkyboxMaterial->SetTexture("u_Environment", 0, std::move(skyboxTexture));
	}

	Scene::Scene(const Scene& other) :
		m_Environment(other.m_Environment),
		m_Registry(),
		m_ViewportRatio(other.m_ViewportRatio),
		m_PhysicsContext(nullptr) // TODO: Make a new instance?
	{
		// TODO: Create same entities as other
		// Copy all components from other
	}

	void Scene::OnUpdate(Timestep ts, bool editor)
	{
		MH_PROFILE_FUNCTION();

		// Delete all entities marked for deletion
		{
			auto view = m_Registry.view<DeleteComponent>();
			m_Registry.destroy(view.begin(), view.end());
		}

		// Sort dirty relationships
		if (!m_Registry.view<DirtyRelationshipComponent>().empty())
		{
			auto group = m_Registry.group<DirtyRelationshipComponent, TransformComponent>();

			group.sort([&](const entt::entity lhs, const entt::entity rhs)
			{
				const RelationshipComponent& clhs = m_Registry.get<RelationshipComponent>(lhs);
				const RelationshipComponent& crhs = m_Registry.get<RelationshipComponent>(rhs);

				size_t lhsDepth = GetDepth(m_Registry, clhs.Parent);
				size_t rhsDepth = GetDepth(m_Registry, crhs.Parent);

				bool lowerParent = lhsDepth < rhsDepth;
				bool lowerAddress = (clhs.Parent == crhs.Parent && &clhs < &crhs);

				return lowerParent || lowerAddress;
			});

			m_Registry.clear<DirtyRelationshipComponent>();
		}

		// Update physics
		if (m_PhysicsContext)
		{
			MH_PROFILE_SCOPE("Scene::OnUpdate - Physics");

			m_PhysicsContext->Update(ts);
		}

		// Update animators
		{
			MH_PROFILE_SCOPE("Scene::OnUpdate - AnimatorComponent");

			m_Registry.view<AnimatorComponent>().each([=](AnimatorComponent& animatorComponent)
			{
				auto& animator = animatorComponent.GetAnimator();

				animator.Update(ts);
			});
		}

		// Update entities of skeleton animators
		{
			MH_PROFILE_SCOPE("Scene::OnUpdate - Animations");

			m_Registry.view<AnimatorComponent, SkeletonComponent, MeshComponent>().each([=](AnimatorComponent& animatorComponent, SkeletonComponent& skeletonComponent, MeshComponent& meshComponent)
			{
				if (!meshComponent.HasMesh())
					return;

				// Skeleton requires a bone mesh
				if (meshComponent.GetPrimitive() != MeshPrimitive::Model)
					return;

				auto& animator = animatorComponent.GetAnimator();

				const auto& translations = animator.GetTranslations();
				const auto& rotations = animator.GetRotations();
				const auto& scales = animator.GetScales();

				auto& boneEntities = skeletonComponent.GetBoneEntities();
				const auto& hierarchy = meshComponent.GetNodeHierarchy();

				if (boneEntities.size() != hierarchy.size())
					return;

				for (size_t i = 0; i < boneEntities.size(); i++)
				{
					auto& boneEntity = boneEntities.at(i);
					if (!boneEntity)
						continue;
					
					TransformComponent* transform = boneEntity.TryGetComponent<TransformComponent>();
					if (!transform)
						continue;

					const auto& index = hierarchy.at(i);

					auto tranIter = translations.find(index.ID);
					if (tranIter != translations.end())
						transform->SetPosition(tranIter->second);

					auto rotIter = rotations.find(index.ID);
					if (rotIter != rotations.end())
						transform->SetRotation(rotIter->second);

					auto sclIter = scales.find(index.ID);
					if (sclIter != scales.end())
						transform->SetScale(sclIter->second);
				}
			});
		}

		// Update matrix transforms
		{
			MH_PROFILE_SCOPE("Scene::OnUpdate - TransformComponent");

			m_Registry.view<RelationshipComponent, TransformComponent>().each([=](RelationshipComponent& relation, TransformComponent& transform)
			{
				if (relation.Parent != entt::null && m_Registry.any_of<TransformComponent>(relation.Parent))
				{
					const auto& parentTransform = m_Registry.get<TransformComponent>(relation.Parent);
					const glm::mat4& parentMatrix = parentTransform.GetModelMatrix();

					transform.UpdateModelMatrix(parentMatrix);
				}
				else
				{
					transform.UpdateModelMatrix(glm::mat4{ 1.0f });
				}
			});
		}

		// Update sound positions
		{
			MH_PROFILE_SCOPE("Scene::OnUpdate - AudioSourceComponent");

			m_Registry.view<TransformComponent, AudioSourceComponent>().each([=](TransformComponent& transformComponent, AudioSourceComponent& audioSourceComponent)
			{
				const glm::mat4& modelMatrix = transformComponent.GetModelMatrix();

				auto source = audioSourceComponent.GetAudioSource();
				source->SetPosition(modelMatrix[3]);
			});
		}

		// Update SkinComponent
		{
			MH_PROFILE_SCOPE("Scene::OnUpdate - SkinComponent");

			m_Registry.view<TransformComponent, SkeletonComponent, MeshComponent>().each([=](TransformComponent& transformComponent, SkeletonComponent& skeletonComponent, MeshComponent& meshComponent)
			{
				if (!meshComponent.HasMesh())
					return;

				// Skeleton requires a bone mesh
				if (meshComponent.GetPrimitive() != MeshPrimitive::Model)
					return;

				const auto& boneEntities = skeletonComponent.GetBoneEntities();
				const auto& skins = meshComponent.GetSkins();
				const auto& hierarchy = meshComponent.GetNodeHierarchy();
				const auto& bones = meshComponent.GetBoneInfo();
				const auto& materials = meshComponent.GetMaterials();
				const auto& submeshMap = meshComponent.GetSubMeshMap();

				for (uint32_t skin : skins)
				{
					if (skin >= boneEntities.size())
						continue;

					// If the mesh doesn't have a material there's nothing to do
					uint32_t submeshID = submeshMap.at(skin);
					if (submeshID >= materials.size())
						continue;

					Asset<Material> material = materials.at(submeshID);
					if (!material)
						continue;

					Entity skinEntity = boneEntities.at(skin);
					if (!skinEntity)
						continue;

					TransformComponent* skinTransform = skinEntity.TryGetComponent<TransformComponent>();
					if (!skinTransform)
						continue;

					glm::mat4 invTransform = glm::inverse(skinTransform->GetModelMatrix());

					for (uint32_t i = 0; i < boneEntities.size(); i++)
					{
						auto& boneEntity = boneEntities.at(i);
						if (!boneEntity)
							continue;

						TransformComponent* boneTransform = boneEntity.TryGetComponent<TransformComponent>();
						if (!boneTransform)
							continue;

						// TODO: Find some better way?
						// Currently the bones are part of the uniform buffer, but that seems a bit unwieldy
						auto boneIter = bones.find(i);
						if (boneIter == bones.end())
							continue;

						const auto& node = hierarchy.at(i);

						glm::mat4 transform = invTransform
							* boneTransform->GetModelMatrix()
							* node.Offset;
						material->SetMat4("Uniforms.BoneMatrices[" + std::to_string(boneIter->second) + "]", transform);
					}
				}
			});
		}

		// Get the listening source
		TransformComponent* listener = nullptr;
		{
			MH_PROFILE_SCOPE("Scene::OnUpdate - AudioListenerComponent");

			m_Registry.view<TransformComponent, AudioListenerComponent>().each([&](TransformComponent& transform, AudioListenerComponent& audio)
			{
				listener = &transform;
			});
		}

		// Update sound buffers
		if (!editor && listener)
			AudioEngine::UpdateSounds(listener->GetModelMatrix());

		// Get the rendering camera
		CameraComponent* mainCamera = nullptr;
		TransformComponent* mainTransform = nullptr;
		{
			MH_PROFILE_SCOPE("Scene::OnUpdate - CameraComponent");

			m_Registry.view<TransformComponent, CameraComponent>().each([&](TransformComponent& transformComponent, CameraComponent& cameraComponent)
			{
				mainCamera = &cameraComponent;
				mainTransform = &transformComponent;
			});
		}

		// Only prepare scene if we actually have something to render to
		if (!editor && mainCamera && mainTransform)
			OnRender(*mainCamera, *mainTransform);
	}

	void Scene::OnRender(Camera& camera, const glm::mat4& cameraTransform)
	{
		MH_PROFILE_FUNCTION();

		// Recalculate all projection matrices, if they've changed
		camera.RecalculateProjectionMatrix();

		// Setup scene lights
		{
			MH_PROFILE_SCOPE("Scene::OnRender - LightComponent");

			// Clear previous lights
			m_Environment.DirectionalLights.clear();
			m_Environment.PointLights.clear();
			m_Environment.SpotLights.clear();

			m_Registry.view<TransformComponent, LightComponent>().each([&](TransformComponent& transformComponent, LightComponent& lightComponent)
			{
				const glm::mat4& modelMatrix = transformComponent;

				glm::vec3 pos = modelMatrix[3];
				glm::quat rot;

				Math::DecomposeRotation(modelMatrix, rot);

				Light& light = lightComponent.GetLight();

				switch (light.GetLightType())
				{
				case Light::LightType::Directional:
					m_Environment.DirectionalLights.push_back({ glm::vec3{ cameraTransform[3] }, rot, light });
					break;
				case Light::LightType::Point:
					m_Environment.PointLights.push_back({ pos, light });
					break;
				case Light::LightType::Spot:
					m_Environment.SpotLights.push_back({ pos, rot, light });
					break;
				}
			});
		}

		// Update particle systems
		/*auto particleView = m_Registry.view<TransformComponent, ParticleSystemComponent>();
		{
			MH_PROFILE_RENDERING_SCOPE("Mahakam::Scene::OnUpdate - ParticleSystemComponent");

			particleView.each([=](auto entity, TransformComponent& transformComponent, ParticleSystemComponent& particleComponent)
			{
				auto& particles = particleComponent.GetParticleSystem();

				particles.Simulate(ts, transform, Renderer);
			});
		}*/

		// Begin the render loop
		{
			MH_PROFILE_SCOPE("Scene::OnRender - Render loop");
			Renderer::BeginScene(camera, cameraTransform, m_Environment);

			// Render each entity with a mesh
			{
				MH_PROFILE_SCOPE("Scene::OnRender - Submit");

				m_Registry.view<TransformComponent, MeshComponent>().each([&](entt::entity entity, TransformComponent& transformComponent, MeshComponent& meshComponent)
				{
					if (!meshComponent.HasMesh())
						return;

					const auto& meshes = meshComponent.GetSubMeshes();
					const auto& materials = meshComponent.GetMaterials();
					uint32_t materialCount = static_cast<uint32_t>(materials.size());

					if (materialCount-- <= 0)
						return;

					if (SkeletonComponent* skeletonComponent = m_Registry.try_get<SkeletonComponent>(entity))
					{
						// Skeleton requires a bone mesh
						if (meshComponent.GetPrimitive() != MeshPrimitive::Model)
							return;

						const auto& boneEntities = skeletonComponent->GetBoneEntities();
						const auto& hierarchy = meshComponent.GetNodeHierarchy();

						if (boneEntities.size() != hierarchy.size())
							return;

						for (auto& [nodeIndex, meshID] : meshComponent.GetSubMeshMap())
						{
							auto& boneEntity = boneEntities.at(nodeIndex);
							if (!boneEntity)
								continue;

							TransformComponent* boneTransform = boneEntity.TryGetComponent<TransformComponent>();
							if (!boneTransform)
								continue;

							const glm::mat4& transform = boneTransform->GetModelMatrix();

							Renderer::Submit(transform, meshes[meshID], materials[meshID < materialCount ? meshID : materialCount]);
						}
					}
					else
					{
						const glm::mat4& modelMatrix = transformComponent.GetModelMatrix();

						for (uint32_t i = 0; i < meshComponent.GetSubMeshCount(); i++)
							Renderer::Submit(modelMatrix, meshes[i], materials[i < materialCount ? i : materialCount]);
					}
				});
			}

			// Render particle systems
			{
				MH_PROFILE_SCOPE("Scene::OnRender - SubmitParticles");

				m_Registry.view<TransformComponent, ParticleSystemComponent>().each([=](TransformComponent& transformComponent, ParticleSystemComponent& particleComponent)
				{
					Renderer::SubmitParticles(transformComponent, particleComponent);
				});
			}

			Renderer::EndScene();
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		// In editor this is called by SceneViewPanel / GameViewPanel
		// In release this is called by EditorLayer.OnWindowResize
		m_ViewportRatio = (float)width / (float)height;

		m_Registry.view<CameraComponent>().each([=](CameraComponent& cameraComponent)
		{
			if (!cameraComponent.HasFixedAspectRatio())
				cameraComponent.GetCamera().SetRatio(m_ViewportRatio);
		});
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		entt::entity handle = m_Registry.create();
		std::string tagName = name.empty() ? "Entity" : name;
		Entity entity(handle, this);
		m_Registry.emplace<TagComponent>(handle, tagName);
		m_Registry.emplace<RelationshipComponent>(handle);
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		entity.Delete();
	}

	//Ref<Scene> Scene::CreateEmpty(PhysicsContext* physics)
	MH_DEFINE_FUNC(Scene::CreateEmpty, Ref<Scene>, PhysicsContext* physics)
	{
		return CreateRef<Scene>(physics);
	};

	//Ref<Scene> Scene::CreateFilepath(const std::string& filepath, PhysicsContext* physics)
	MH_DEFINE_FUNC(Scene::CreateFilepath, Ref<Scene>, const std::string& filepath, PhysicsContext* physics)
	{
		return CreateRef<Scene>(filepath, physics);
	};
}