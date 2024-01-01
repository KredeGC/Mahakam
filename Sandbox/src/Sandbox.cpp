#include "sbpch.h"

#define MH_STANDALONE_TITLE "Sandbox"
#define MH_STANDALONE_ICON "internal/icons/icon-64.png"

#include <Mahakam/MahakamRuntime.h>
#include <Mahakam/MahakamEntrypoint.h>

using namespace Mahakam;

struct MH_EXPORTED RotatorComponent
{
	float rotation = 0.0f;
	float rotationSpeed = 10.0f;

	RotatorComponent() {}
};

struct MH_EXPORTED CamerControllerComponent
{
	float moveSpeed = 5.0f;
	float rotationSpeed = 2.0f;

	CamerControllerComponent() {}
};

MH_EXTERN_EXPORTED void Load()
{
	MH_TRACE("DLL Loaded!");

	// Setup render passes for pixel renderer
	/*Renderer::SetRenderPasses({
		CreateRef<TexelGeometryPass>(),
		CreateRef<TexelLightingPass>(),
		CreateRef<PixelationPass>(),
		CreateRef<ParticleRenderPass>(),
		CreateRef<TonemappingRenderPass>() });*/

#pragma region Rotator
	ComponentRegistry::ComponentInterface rotatorInterface;
	rotatorInterface.SetComponent<RotatorComponent>();

	rotatorInterface.SetEditor(nullptr, [](Entity entity)
	{
		RotatorComponent& rotator = entity.GetComponent<RotatorComponent>();

		ImGui::DragFloat("Rotation speed", &rotator.rotationSpeed, 0.1f);
	});

	ComponentRegistry::RegisterComponent("Rotator", rotatorInterface);
#pragma endregion
}

MH_EXTERN_EXPORTED void Run(Scene* scene)
{
	// Scene camera
	Entity cameraEntity = scene->CreateEntity("Main Camera");
	cameraEntity.AddComponent<CameraComponent>(Camera::ProjectionType::Perspective, glm::radians(45.0f), 0.01f, 100.0f);
	cameraEntity.AddComponent<TransformComponent>().SetPosition({ 4.5f, 4.5f, 12.5f });
	cameraEntity.AddComponent<AudioListenerComponent>();
	cameraEntity.AddComponent<CamerControllerComponent>();


	// Directional light
	Entity mainLightEntity = scene->CreateEntity("Main Light");
	mainLightEntity.AddComponent<LightComponent>(Light::LightType::Directional, 20.0f, glm::vec3(1.0f, 1.0f, 1.0f), true);
	mainLightEntity.AddComponent<TransformComponent>().SetRotation(glm::quat({ -0.7f, -3.0f, 0.0f }));


	// Spot light
	Entity pointLightEntity = scene->CreateEntity("Spot Light");
	pointLightEntity.AddComponent<LightComponent>(Light::LightType::Spot, glm::radians(45.0f), 10.0f, glm::vec3(1.0f, 1.0f, 1.0f), true);
	pointLightEntity.AddComponent<TransformComponent>().SetPosition({ 1.0f, 2.5f, 4.0f });
	pointLightEntity.GetComponent<TransformComponent>().SetRotation(glm::quat({ glm::radians(-150.0f), glm::radians(180.0f), 0.0f }));


	// Setup plane
	Asset<Mesh> planeMesh = Asset<Mesh>(556425745901094492ull);

	/*Ref<SubMesh> planeMesh = SubMesh::CreatePlane(2, 2);

	Asset<Material> planeMaterial = Asset<Material>("import/assets/materials/BrickPlane.material.import");*/

	Entity planeEntity = scene->CreateEntity("Plane");
	planeEntity.AddComponent<MeshComponent>(planeMesh);
	planeEntity.AddComponent<TransformComponent>().SetPosition({ 0.0f, -1.0f, 0.0f });
	planeEntity.GetComponent<TransformComponent>().SetScale({ 30.0f, 30.0f, 30.0f });


	// Create particle system
	Entity particleEntity = scene->CreateEntity("Particle System");
	particleEntity.AddComponent<ParticleSystemComponent>();
	particleEntity.AddComponent<TransformComponent>().SetPosition({ 0.0f, 0.0f, 1.0f });


#if 0
	// Create backpack model
	Asset<Mesh> backpackModel = Mesh::LoadMesh("resource/models/backpack.gltf");

	Asset<Material> backpackMaterial = Asset<Material>("assets/materials/Backpack.material.asset");

	// Create backpack entity
	Entity backpackEntity = scene->CreateEntity("Bacpack");
	backpackEntity.AddComponent<MeshComponent>(backpackModel, backpackMaterial);
	backpackEntity.AddComponent<TransformComponent>().SetPosition({ 2.5f, 4.0f, 7.5f });
	backpackEntity.AddComponent<RotatorComponent>();
#endif


#if 1
	Entity animatedArchive = scene->CreateEntity("Animated Skeletons");

	// Create glTF skinned model
	Asset<BoneMesh> skinnedModel = Asset<BoneMesh>(11945816250811211738ull);
	//Asset<Mesh> skinnedModel = Mesh::LoadMesh("resource/models/mannequin_clap.glb");
	Asset<Animation> skinnedAnimation = Asset<Animation>(1494460746093480384ull);
	//Asset<Animation> skinnedAnimation = Animation::Load("resource/models/mannequin_clap.glb");

	//Asset<Material> skinnedMaterial = Asset<Material>("assets/materials/Skinned.material.asset");

	// Create backpack entity
	Entity skinnedEntity = scene->CreateEntity("Skinned glTF");
	skinnedEntity.SetParent(animatedArchive);
	skinnedEntity.AddComponent<MeshComponent>(skinnedModel);
	skinnedEntity.AddComponent<TransformComponent>().SetPosition({ 2.5f, 4.0f, 7.5f });
	skinnedEntity.AddComponent<AnimatorComponent>().GetAnimator().SetAnimation(skinnedAnimation);
	SkeletonComponent& skin = skinnedEntity.AddComponent<SkeletonComponent>();

	skin.CreateBoneEntities(skinnedEntity, skinnedModel->NodeHierarchy);
#endif


#if 0
	// Create piano sound entity
	Asset<Sound> pianoSound = Asset<Sound>("assets/sounds/piano.wav.asset");

	Entity pianoEntity = scene->CreateEntity("Piano");
	pianoEntity.AddComponent<TransformComponent>().SetPosition({ 2.5f, 4.0f, 7.5f });
	AudioSourceComponent& pianoSource = pianoEntity.AddComponent<AudioSourceComponent>();
	pianoSource.SetSpatialBlend(1.0f);
	pianoSource.SetSound(pianoSound);
	pianoSource.Play();


	// Create vespa sound entity
	Asset<Sound> vespaSound = Asset<Sound>("assets/sounds/vespa.wav.asset");

	Entity vespaEntity = scene->CreateEntity("Vespa Sound");
	vespaEntity.AddComponent<TransformComponent>().SetPosition({ 0.0f, 1.0f, 2.0f });
	AudioSourceComponent& vespaSource = vespaEntity.AddComponent<AudioSourceComponent>();
	vespaSource.SetSound(vespaSound);
	vespaSource.Play();
#endif


#if 1
	// Create base collection entity to store in
	Asset<Mesh> sphereMesh = Asset<Mesh>(14539941857077216621ull);

	// Create base collection entity to store in
	Entity sphereCollection = scene->CreateEntity("Spheres");
	sphereCollection.AddComponent<TransformComponent>().SetEulerangles({ 0.0f, 90.0f, 0.0f });

	// Create scene entities
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			// Setup material with texture
			Asset<Material> material = Material::Copy(sphereMesh->GetProps().Materials[0]);
			material->SetFloat("Uniforms.MetallicMul", y / 10.0f);
			material->SetFloat("Uniforms.RoughnessMul", x / 10.0f);

			// Create entity
			Entity entity = scene->CreateEntity(std::string("Sphere ") + std::to_string(x) + std::string(",") + std::to_string(y));
			entity.SetParent(sphereCollection);
			entity.AddComponent<MeshComponent>(Mesh::Copy(sphereMesh), material);
			entity.AddComponent<TransformComponent>().SetPosition({ x, y, 0.0f });

			if (x == 0 && y == 0)
				entity.GetComponent<TransformComponent>().SetScale({ 2.0f, 1.0f, 1.0f });

			//sphereCollection = entity; // TEMPORARY: Just to see the depth
		}
	}
#endif

	Entity entity1 = scene->CreateEntity("ID 1");
	Entity entity2 = scene->CreateEntity("ID 2");
	Entity entity3 = scene->CreateEntity("ID 3");
	Entity entity4 = scene->CreateEntity("ID 4");
	Entity entity5 = scene->CreateEntity("ID 5");
	Entity entity6 = scene->CreateEntity("ID 6");
	Entity entity7 = scene->CreateEntity("ID 7");
	Entity entity8 = scene->CreateEntity("ID 8");

	entity1.SetParent(entity6);
	entity2.SetParent(entity1);
	entity3.SetParent(entity4);
	entity4.SetParent(entity1);
	entity5.SetParent(entity2);
	entity7.SetParent(entity5);
	entity8.SetParent(entity4);
}


MH_EXTERN_EXPORTED void Update(Scene* scene, Timestep dt)
{
	MH_PROFILE_FUNCTION();

	// Update rotator component
	scene->ForEach<TransformComponent, RotatorComponent>([&](auto entity, TransformComponent& transform, RotatorComponent& rotator)
	{
		rotator.rotation += dt * rotator.rotationSpeed;
		transform.SetRotation(glm::quat(glm::vec3{ 0.0f, glm::radians(rotator.rotation), 0.0f }));
	});


	// Update bone entities
	/*for (auto& [id, boneEntity] : boneEntities)
	{
		auto& bone = boneIDs[id];
		glm::mat4 transform = glm::inverse(skinnedEntity.GetComponent<TransformComponent>().GetModelMatrix())
			* boneEntity.GetComponent<TransformComponent>().GetModelMatrix()
			* boneIDs[id].offset;
		skinnedMaterial->SetMat4("finalBonesMatrices[" + std::to_string(bone.id) + "]", transform);
	}*/


	// Update Camera controller
	scene->ForEach<TransformComponent, CamerControllerComponent>([&](auto entity, TransformComponent& transform, CamerControllerComponent& controller)
	{
		float speed = controller.moveSpeed * dt;
		float rotationSpeed = controller.rotationSpeed * dt;

		if (Input::IsKeyPressed(Key::LEFT_SHIFT))
			speed *= 0.01f;

		glm::vec3 eulerAngles = transform.GetEulerAngles();

		// Camera rotation
		if (Input::IsKeyPressed(Key::LEFT))
		{
			eulerAngles.y += rotationSpeed;
			transform.SetEulerangles(eulerAngles);
		}
		else if (Input::IsKeyPressed(Key::RIGHT))
		{
			eulerAngles.y -= rotationSpeed;
			transform.SetEulerangles(eulerAngles);
		}

		if (Input::IsKeyPressed(Key::UP))
		{
			eulerAngles.x += rotationSpeed;
			transform.SetEulerangles(eulerAngles);
		}
		else if (Input::IsKeyPressed(Key::DOWN))
		{
			eulerAngles.x -= rotationSpeed;
			transform.SetEulerangles(eulerAngles);
		}

		// Camera movement
		if (Input::IsKeyPressed(Key::A))
			transform.SetPosition(transform.GetPosition() - glm::vec3(speed) * transform.GetRight());
		else if (Input::IsKeyPressed(Key::D))
			transform.SetPosition(transform.GetPosition() + glm::vec3(speed) * transform.GetRight());

		if (Input::IsKeyPressed(Key::W))
			transform.SetPosition(transform.GetPosition() - glm::vec3(speed) * transform.GetForward());
		else if (Input::IsKeyPressed(Key::S))
			transform.SetPosition(transform.GetPosition() + glm::vec3(speed) * transform.GetForward());

		if (Input::IsKeyPressed(Key::Q))
			transform.SetPosition(transform.GetPosition() - glm::vec3(speed) * transform.GetUp());
		else if (Input::IsKeyPressed(Key::E))
			transform.SetPosition(transform.GetPosition() + glm::vec3(speed) * transform.GetUp());
	});
}

MH_EXTERN_EXPORTED void Stop(Scene* scene)
{
	// TODO: Remove entities again
}

MH_EXTERN_EXPORTED void Unload()
{
	MH_TRACE("DLL Unloaded!");

#pragma region Rotator
	ComponentRegistry::DeregisterComponent("Rotator");
#pragma endregion
}