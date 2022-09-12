#include "Mahakam/mhpch.h"
#include "Scene.h"

#include "Components/AnimatorComponent.h"
#include "Components/AudioListenerComponent.h"
#include "Components/AudioSourceComponent.h"
#include "Components/CameraComponent.h"
#include "Components/FlagComponents.h"
#include "Components/LightComponent.h"
#include "Components/MeshComponent.h"
#include "Components/ParticleSystemComponent.h"
#include "Components/RelationshipComponent.h"
#include "Components/SkinComponent.h"
#include "Components/TagComponent.h"
#include "Components/TransformComponent.h"
#include "Entity.h"

#include "Mahakam/Asset/AssetDatabase.h"

#include "Mahakam/Audio/AudioEngine.h"

#include "Mahakam/Core/FileUtility.h"

#include "Mahakam/Math/Math.h"

#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/RenderData.h"
#include "Mahakam/Renderer/Renderer.h"
#include "Mahakam/Renderer/Shader.h"
#include "Mahakam/Renderer/Texture.h"

#include <yaml-cpp/yaml.h>

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



	static uint16_t RecursiveDepth(entt::registry& registry, const RelationshipComponent& relation)
	{
		if (relation.Parent != entt::null)
			return 1 + RecursiveDepth(registry, registry.get<RelationshipComponent>(relation.Parent));
		else
			return 0;
	}

	Scene::Scene()
	{
		MH_PROFILE_FUNCTION();

		Asset<Shader> skyboxShader = Asset<Shader>(Shader::Create("assets/shaders/Skybox.shader"));
		m_Environment.SkyboxMaterial = Asset<Material>(Material::Create(skyboxShader));
		m_Environment.SkyboxMaterial->SetTexture("u_Environment", 0, Asset<TextureCube>(GL::GetTextureCubeWhite()));
	}

	Scene::Scene(const std::string& filepath)
	{
		MH_PROFILE_FUNCTION();

		Asset<Texture> skyboxTexture = Asset<TextureCube>(TextureCube::Create(filepath, { 4096, TextureFormat::RG11B10F }));
		m_Environment.IrradianceMap = Asset<TextureCube>(TextureCube::Create(filepath, { 64, TextureFormat::RG11B10F, TextureCubePrefilter::Convolute, false }));
		m_Environment.SpecularMap = Asset<TextureCube>(TextureCube::Create(filepath, { 512, TextureFormat::RG11B10F, TextureCubePrefilter::Prefilter, true }));

		//skyboxIrradiance = AssetDatabase::CreateOrLoadAsset<TextureCube>(filepath + ".irradiance", skyboxTexture, false, TextureCubePrefilter::Convolute, { 64, TextureFormat::RG11B10F, false });
		//skyboxSpecular = AssetDatabase::CreateOrLoadAsset<TextureCube>(filepath + ".specular", skyboxTexture, true, TextureCubePrefilter::Prefilter, { 512, TextureFormat::RG11B10F, true });

		Asset<Shader> skyboxShader = Asset<Shader>(Shader::Create("assets/shaders/Skybox.shader"));
		m_Environment.SkyboxMaterial = Asset<Material>(Material::Create(skyboxShader));
		m_Environment.SkyboxMaterial->SetTexture("u_Environment", 0, skyboxTexture);
	}

	Scene::~Scene() {}

	void Scene::OnUpdate(Timestep ts, bool editor)
	{
		MH_PROFILE_FUNCTION();

		// Update animators
		{
			MH_PROFILE_SCOPE("Scene::OnUpdate - AnimatorComponent");

			m_Registry.view<AnimatorComponent, SkinComponent, MeshComponent>().each([=](AnimatorComponent& animatorComponent, SkinComponent& skinComponent, MeshComponent& meshComponent)
			{
				auto& animator = animatorComponent.GetAnimator();

				animator.Update(ts);

				const auto& translations = animator.GetTranslations();
				const auto& rotations = animator.GetRotations();
				const auto& scales = animator.GetScales();

				auto& boneEntities = skinComponent.GetBoneEntities();
				const auto& hierarchy = meshComponent.GetNodeHierarchy();

				if (boneEntities.size() != hierarchy.size())
				{
					boneEntities.resize(hierarchy.size());
					return;
				}

				for (size_t i = 0; i < boneEntities.size(); i++)
				{
					auto& index = hierarchy.at(i);
					auto& boneEntity = boneEntities.at(i);

					if (boneEntity)
					{
						if (TransformComponent* transform = boneEntity.TryGetComponent<TransformComponent>())
						{
							auto tranIter = translations.find(index.id);
							if (tranIter != translations.end())
								transform->SetPosition(tranIter->second);

							auto rotIter = rotations.find(index.id);
							if (rotIter != rotations.end())
								transform->SetRotation(rotIter->second);

							auto sclIter = scales.find(index.id);
							if (sclIter != scales.end())
								transform->SetScale(sclIter->second);
						}
					}
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
				// TODO: Use the model matrix instead. Helps with parenting
				auto source = audioSourceComponent.GetAudioSource();
				source->SetPosition(transformComponent.GetPosition());
			});
		}

		// Update SkinComponent
		{
			MH_PROFILE_SCOPE("Scene::OnUpdate - SkinComponent");

			m_Registry.view<TransformComponent, SkinComponent, MeshComponent>().each([=](TransformComponent& transformComponent, SkinComponent& skinComponent, MeshComponent& meshComponent)
			{
				if (!meshComponent.HasMesh()) return;

				const auto& boneEntities = skinComponent.GetBoneEntities();
				const auto& hierarchy = meshComponent.GetNodeHierarchy();
				const auto& bones = meshComponent.GetBoneInfo();
				const auto& materials = meshComponent.GetMaterials();

				glm::mat4 invTransform = glm::inverse(transformComponent.GetModelMatrix());

				for (auto& material : materials)
				{
					for (size_t i = 0; i < boneEntities.size(); i++)
					{
						auto& boneEntity = boneEntities.at(i);
						if (boneEntity)
						{
							if (TransformComponent* boneTransform = boneEntity.TryGetComponent<TransformComponent>())
							{
								auto& node = hierarchy.at(i);

								// TODO: Replace the mat4 array with a UniformBuffer
								auto boneIter = bones.find(node.name);
								if (boneIter != bones.end())
								{
									glm::mat4 transform = invTransform
										* boneTransform->GetModelMatrix()
										* node.offset;
									material->SetMat4("finalBonesMatrices[" + std::to_string(boneIter->second) + "]", transform);
								}
							}
						}
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
			MH_PROFILE_SCOPE("Scene::OnUpdate - LightComponent");

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
			MH_PROFILE_SCOPE("Scene::OnUpdate - Render loop");
			Renderer::BeginScene(camera, cameraTransform, m_Environment);

			// Render each entity with a mesh
			{
				MH_PROFILE_SCOPE("Scene::OnUpdate - Submit");

				m_Registry.view<TransformComponent, MeshComponent>().each([&](entt::entity entity, TransformComponent& transformComponent, MeshComponent& meshComponent)
				{
					if (!meshComponent.HasMesh()) return;

					const auto& meshes = meshComponent.GetSubMeshes();
					const auto& materials = meshComponent.GetMaterials();
					int materialCount = (int)materials.size() - 1;

					if (materialCount >= 0)
					{
						if (SkinComponent* skinComponent = m_Registry.try_get<SkinComponent>(entity))
						{
							const auto& boneEntities = skinComponent->GetBoneEntities();
							const auto& hierarchy = meshComponent.GetNodeHierarchy();

							for (size_t i = 0; i < hierarchy.size(); i++)
							{
								auto& index = hierarchy.at(i);
								auto& boneEntity = boneEntities.at(i);

								if (boneEntity && index.mesh > -1)
								{
									if (TransformComponent* boneTransform = boneEntity.TryGetComponent<TransformComponent>())
									{
										// TODO: Undo the skin transformation instead. This doesn't actually seem to work though
										// I've yet to make the inverse skin transform to work. For now it has a toggle instead
										glm::mat4 transform;
										if (skinComponent->HasTargetOrigin())
											transform = boneEntity.GetComponent<TransformComponent>().GetModelMatrix()
											* index.offset;
										else
											transform = transformComponent.GetModelMatrix();

										Renderer::Submit(transform, meshes[index.mesh], materials[index.mesh < materialCount ? index.mesh : materialCount].Get());
									}
								}
							}
						}
						else
						{
							glm::mat4 modelMatrix = transformComponent.GetModelMatrix();

							for (int i = 0; i < meshComponent.GetSubMeshCount(); i++)
								Renderer::Submit(modelMatrix, meshes[i], materials[i < materialCount ? i : materialCount].Get());
						}
					}
				});
			}

			// Render particle systems
			{
				MH_PROFILE_SCOPE("Scene::OnUpdate - SubmitParticles");

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

		auto cameras = m_Registry.view<CameraComponent>();
		for (auto& entity : cameras)
		{
			CameraComponent& cameraComponent = cameras.get<CameraComponent>(entity);

			if (!cameraComponent.HasFixedAspectRatio())
			{
				cameraComponent.GetCamera().SetRatio(m_ViewportRatio);
			}
		}
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		std::string tagName = name.empty() ? "Entity" : name;
		Entity entity(m_Registry.create(), this);
		auto& tag = entity.AddComponent<TagComponent>(tagName);
		auto& relation = entity.AddComponent<RelationshipComponent>();
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::Sort()
	{
		m_Registry.sort<RelationshipComponent>([&](const RelationshipComponent& clhs, const RelationshipComponent& crhs)
		{
			uint16_t lhsDepth = RecursiveDepth(m_Registry, clhs);
			uint16_t rhsDepth = RecursiveDepth(m_Registry, crhs);

			// TODO: Store the depth somehow
			// Possibly sort by parents as well

			bool lowerParent = lhsDepth < rhsDepth;
			bool lowerAddress = (clhs.Parent == crhs.Parent && &clhs < &crhs);

			return lowerParent || lowerAddress;
		});

		//registry.sort<RelationshipComponent, TransformComponent>();

		m_Registry.sort<TransformComponent>([&](const entt::entity lhs, const entt::entity rhs)
		{
			auto& clhs = m_Registry.get<RelationshipComponent>(lhs);
			auto& crhs = m_Registry.get<RelationshipComponent>(rhs);

			uint16_t lhsDepth = RecursiveDepth(m_Registry, clhs);
			uint16_t rhsDepth = RecursiveDepth(m_Registry, crhs);

			// TODO: Store the depth somehow
			// Possibly sort by parents as well

			bool lowerParent = lhsDepth < rhsDepth;
			bool lowerAddress = (clhs.Parent == crhs.Parent && &clhs < &crhs);

			return lowerParent || lowerAddress;
		});
	}

	//Ref<Scene> Scene::CreateEmpty()
	MH_DEFINE_FUNC(Scene::CreateEmpty, Ref<Scene>)
	{
		return CreateRef<Scene>();
	};

	//Ref<Scene> Scene::CreateFilepath(const std::string& filepath)
	MH_DEFINE_FUNC(Scene::CreateFilepath, Ref<Scene>, const std::string& filepath)
	{
		return CreateRef<Scene>(filepath);
	};
}