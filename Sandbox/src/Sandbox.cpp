#include "sbpch.h"

#define MH_STANDALONE_TITLE "Title"
#define MH_STANDALONE_ICON "import/internal/icons/icon-editor.png"

#include <MahakamRuntime.h>

#if MH_STANDALONE
#include <MahakamEntrypoint.h>
#endif

using namespace Mahakam;

struct EXPORTED RotatorComponent
{
	float rotation = 0.0f;;
	float rotationSpeed = 10.0f;

	RotatorComponent() {}
};

struct EXPORTED CamerControllerComponent
{
	float moveSpeed = 5.0f;
	float rotationSpeed = 2.0f;

	CamerControllerComponent() {}
};

EXTERN_EXPORTED void Load(ImGuiContext* context, void*** funcPtrs)
{
	MH_RUNTIME_LOAD(context, funcPtrs);

	// Setup render passes for the default renderer
	/*Renderer::SetRenderPasses({
		CreateRef<GeometryRenderPass>(),
		CreateRef<LightingRenderPass>(),
		CreateRef<ParticleRenderPass>(),
		CreateRef<TonemappingRenderPass>() });*/

	// Setup render passes for pixel renderer
	/*Renderer::SetRenderPasses({
		CreateRef<TexelGeometryPass>(),
		CreateRef<TexelLightingPass>(),
		CreateRef<PixelationPass>(),
		CreateRef<ParticleRenderPass>(),
		CreateRef<TonemappingRenderPass>() });*/

	Asset<Texture2D> testTex = Texture2D::Create("assets/textures/brick/brick_albedo.png", { TextureFormat::SRGB_DXT1, TextureFilter::Point });
	testTex->GetRendererID();

	MH_CORE_TRACE("DLL Loaded!");


	// TestComponent
	ComponentRegistry::ComponentInterface rotatorInterface;
	rotatorInterface.HasComponent = [](Entity entity) { return entity.HasComponent<RotatorComponent>(); };
	rotatorInterface.AddComponent = [](Entity entity) { entity.AddComponent<RotatorComponent>(); };
	rotatorInterface.RemoveComponent = [](Entity entity) { entity.RemoveComponent<RotatorComponent>(); };
	rotatorInterface.OnInspector = [](Entity entity)
	{
		RotatorComponent& rotator = entity.GetComponent<RotatorComponent>();

		ImGui::DragFloat("Rotation speed", &rotator.rotationSpeed, 0.1f);
	};

	ComponentRegistry::RegisterComponent("Rotater", rotatorInterface);


	// TODO: Add components to scene list of serializable components
	// TODO: Add components to editor list for inspector
}

EXTERN_EXPORTED void Run(Scene* scene)
{
	// Create light entity
	Entity entity = scene->CreateEntity("DLL Light Test");
	entity.AddComponent<LightComponent>(Light::LightType::Spot, glm::radians(45.0f), 10.0f, glm::vec3(1.0f, 1.0f, 1.0f), true);
	entity.GetComponent<TransformComponent>().SetPosition({ 0.0f, 0.0f, 1.0f });

	// Create skinned material
	// Asset<Material> skinnedMaterial = Material::Create(skinnedShader);
	// skinnedMaterial->SetFloat3("u_Color", { 0.68f, 0.44f, 0.22f });
	// skinnedMaterial->SetFloat("u_Metallic", 1.0f);
	// skinnedMaterial->SetFloat("u_Roughness", 0.4f);
	Asset<Material> skinnedMaterial = Asset<Material>("import/assets/materials/Skinned.material.import");

#if 1
	// Create animation and model
	SkinnedMesh skinnedModel = Mesh::LoadModel("assets/models/mannequin_clap.fbx");
	Asset<Animation> animation = Animation::Load("assets/models/mannequin_clap.fbx", skinnedModel);

	// Create skinned entities
	for (int x = 0; x < 2; x++)
	{
		for (int z = 0; z < 2; z++)
		{
			Entity animatedEntity = scene->CreateEntity("DLL Animated");
			animatedEntity.AddComponent<MeshComponent>(skinnedModel, skinnedMaterial);
			animatedEntity.GetComponent<TransformComponent>().SetPosition({ x * 5.0f, 1.5f, 5.0f + z * 5.0f });
			animatedEntity.GetComponent<TransformComponent>().SetScale({ 0.02f, 0.02f, 0.02f });
			animatedEntity.AddComponent<AnimatorComponent>(animation);

			if (z == 1)
				animatedEntity.GetComponent<TransformComponent>().SetEulerangles({ 0.0f, 3.1415f, 0.0f });
		}
	}
#endif


	// Scene camera
	Entity cameraEntity = scene->CreateEntity("Main Camera");
	cameraEntity.AddComponent<CameraComponent>(Camera::ProjectionType::Perspective, glm::radians(45.0f), 0.01f, 100.0f);
	cameraEntity.GetComponent<TransformComponent>().SetPosition({ 4.5f, 4.5f, 12.5f });
	cameraEntity.AddComponent<AudioListenerComponent>();
	cameraEntity.AddComponent<CamerControllerComponent>();


	// Directional light
	Entity mainLightEntity = scene->CreateEntity("Main Light");
	mainLightEntity.AddComponent<LightComponent>(Light::LightType::Directional, 20.0f, glm::vec3(1.0f, 1.0f, 1.0f), true);
	mainLightEntity.GetComponent<TransformComponent>().SetRotation(glm::quat({ -0.7f, -3.0f, 0.0f }));


	// Spot
	Entity pointLightEntity = scene->CreateEntity("Spot Light");
	pointLightEntity.AddComponent<LightComponent>(Light::LightType::Spot, glm::radians(45.0f), 10.0f, glm::vec3(1.0f, 1.0f, 1.0f), true);
	pointLightEntity.GetComponent<TransformComponent>().SetPosition({ 1.0f, 2.5f, 4.0f });
	pointLightEntity.GetComponent<TransformComponent>().SetRotation(glm::quat({ glm::radians(-150.0f), glm::radians(180.0f), 0.0f }));


	// Setup plane
	Asset<Texture2D> brickAlbedo = Asset<Texture2D>("import/assets/textures/brick/brick_albedo.png.import");
	Asset<Texture> brickBump = Asset<Texture2D>("import/assets/textures/brick/brick_bump.png.import");
	Asset<Texture> brickRoughness = Asset<Texture2D>("import/assets/textures/brick/brick_roughness.png.import");
	/*Asset<Texture> brickAlbedo = Texture2D::Create("assets/textures/brick/brick_albedo.png", { 128, 128, TextureFormat::SRGB_DXT1, TextureFilter::Point });
	Asset<Texture> brickBump = Texture2D::Create("assets/textures/brick/brick_bump.png", { 128, 128, TextureFormat::RG_BC5, TextureFilter::Point });
	Asset<Texture> brickRoughness = Texture2D::Create("assets/textures/brick/brick_roughness.png", { 128, 128, TextureFormat::R_BC4, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });*/
	/*Ref<Texture> brickAlbedo = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/brick/brick_albedo.png", false, { 128, 128, TextureFormat::SRGB_DXT1, TextureFilter::Point });
	Ref<Texture> brickBump = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/brick/brick_bump.png", false, { 128, 128, TextureFormat::RG_BC5, TextureFilter::Point });
	Ref<Texture> brickRoughness = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/brick/brick_roughness.png", false, { 128, 128, TextureFormat::R_BC4, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });*/
	Asset<Mesh> planeMesh = Mesh::CreatePlane(2, 2);

	Asset<Material> planeMaterial = Asset<Material>("import/assets/materials/BrickPlane.material.import");
	/*Asset<Material> planeMaterial = Material::Create(textureShader);
	planeMaterial->SetTexture("u_Albedo", 0, brickAlbedo);
	planeMaterial->SetTexture("u_Bump", 0, brickBump);
	planeMaterial->SetTexture("u_Metallic", 0, GL::GetTexture2DBlack());
	planeMaterial->SetTexture("u_Roughness", 0, brickRoughness);
	planeMaterial->SetTexture("u_Occlussion", 0, GL::GetTexture2DWhite());*/

	Entity planeEntity = scene->CreateEntity("Plane");
	planeEntity.AddComponent<MeshComponent>(planeMesh, planeMaterial);
	planeEntity.GetComponent<TransformComponent>().SetPosition({ 0.0f, -1.0f, 0.0f });
	planeEntity.GetComponent<TransformComponent>().SetScale({ 30.0f, 30.0f, 30.0f });


	// Create particle system
	Entity particleEntity = scene->CreateEntity("Particle System");
	particleEntity.AddComponent<ParticleSystemComponent>();
	particleEntity.GetComponent<TransformComponent>().SetPosition({ 0.0f, 0.0f, 1.0f });


#if 0
	// Create backpack textures
	Ref<Texture2D> backpackDiffuse = Texture2D::Create("assets/textures/backpack/diffuse.jpg", { 4096, 4096, TextureFormat::SRGB_DXT1 });
	Ref<Texture2D> backpackOcclussion = Texture2D::Create("assets/textures/backpack/ao.jpg", { 4096, 4096, TextureFormat::R_BC4 });
	Ref<Texture2D> backpackBump = Texture2D::Create("assets/textures/backpack/normal.png", { 4096, 4096, TextureFormat::RG_BC5 });
	Ref<Texture2D> backpackMetallic = Texture2D::Create("assets/textures/backpack/specular.jpg", { 4096, 4096, TextureFormat::R_BC4 });
	Ref<Texture2D> backpackRoughness = Texture2D::Create("assets/textures/backpack/roughness.jpg", { 4096, 4096, TextureFormat::R_BC4 });
	/*Ref<Texture2D> backpackDiffuse = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/diffuse.jpg", false, { 4096, 4096, TextureFormat::SRGB_DXT1 });
	Ref<Texture> backpackOcclussion = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/ao.jpg", false, { 4096, 4096, TextureFormat::R_BC4 });
	Ref<Texture> backpackBump = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/normal.png", false, { 4096, 4096, TextureFormat::RG_BC5 });
	Ref<Texture> backpackMetallic = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/specular.jpg", false, { 4096, 4096, TextureFormat::R_BC4 });
	Ref<Texture> backpackRoughness = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/roughness.jpg", false, { 4096, 4096, TextureFormat::R_BC4 });*/

	// Create backpack model
	SkinnedMesh backpackModel = Mesh::LoadModel("assets/models/backpack.obj");

	// Create backpack material
	Ref<Material> backpackMaterial = Material::Create(textureShader);
	backpackMaterial->SetTexture("u_Albedo", 0, backpackDiffuse);
	backpackMaterial->SetTexture("u_Bump", 0, backpackBump);
	backpackMaterial->SetTexture("u_Metallic", 0, backpackMetallic);
	backpackMaterial->SetTexture("u_Roughness", 0, backpackRoughness);
	backpackMaterial->SetTexture("u_Occlussion", 0, backpackOcclussion);

	// Create backpack entity
	Entity backpackEntity = scene->CreateEntity("Bacpack");
	backpackEntity.AddComponent<MeshComponent>(backpackModel, backpackMaterial);
	backpackEntity.GetComponent<TransformComponent>().SetPosition({ 2.5f, 4.0f, 7.5f });
	backpackEntity.AddComponent<RotatorComponent>();
#endif


	// Create becret sound entity
	Asset<Sound> becretSound = Asset<Sound>("import/assets/sounds/piano.wav.import");

	Entity becretEntity = scene->CreateEntity("Piano");
	becretEntity.GetComponent<TransformComponent>().SetPosition({ 2.5f, 4.0f, 7.5f });
	AudioSourceComponent& becretSource = becretEntity.AddComponent<AudioSourceComponent>();
	becretSource.SetSpatialBlend(1.0f);
	becretSource.SetSound(becretSound);
	becretSource.Play();


	// Create fern sound entity
	Asset<Sound> fernSound = Asset<Sound>("import/assets/sounds/vespa.wav.import");

	Entity fernEntity = scene->CreateEntity("Vespa Sound");
	fernEntity.GetComponent<TransformComponent>().SetPosition({ 0.0f, 1.0f, 2.0f });
	AudioSourceComponent& fernSource = fernEntity.AddComponent<AudioSourceComponent>();
	fernSource.SetSound(fernSound);
	fernSource.Play();


	// Create mesh & base material
	Asset<Mesh> sphereMesh = Mesh::CreateCubeSphere(9);
	//Asset<Material> baseMaterial = Material::Create(colorShader);
	Asset<Material> baseMaterial = Asset<Material>("import/assets/materials/WhiteDiffuse.material.import");
	//baseMaterial->SetFloat3("u_Color", { 1.0f, 1.0f, 1.0f });

	// Create scene entities
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			// Setup material with texture
			Asset<Material> material = Material::Copy(baseMaterial);
			material->SetFloat("u_MetallicMul", y / 10.0f);
			material->SetFloat("u_RoughnessMul", x / 10.0f);

			// Create entity
			Entity entity = scene->CreateEntity(std::string("Sphere ") + std::to_string(x) + std::string(",") + std::to_string(y));
			entity.AddComponent<MeshComponent>(sphereMesh, material);
			entity.GetComponent<TransformComponent>().SetPosition({ x, y, 0.0f });
		}
	}
}


EXTERN_EXPORTED void Update(Scene* scene, Timestep dt)
{
	MH_PROFILE_FUNCTION();

	// Update rotator component
	scene->ForEach<TransformComponent, RotatorComponent>([&](auto entity, TransformComponent& transform, RotatorComponent& rotator)
	{
		rotator.rotation += dt * rotator.rotationSpeed;
		transform.SetRotation(glm::quat(glm::vec3{ 0.0f, glm::radians(rotator.rotation), 0.0f }));
	});


	// Update Camera controller
	scene->ForEach<TransformComponent, CamerControllerComponent>([&](auto entity, TransformComponent& transform, CamerControllerComponent& controller)
	{
		float speed = controller.moveSpeed * dt;
		float rotationSpeed = controller.rotationSpeed * dt;

		if (Input::IsKeyPressed(MH_KEY_LEFT_SHIFT))
			speed *= 0.01f;

		glm::vec3 eulerAngles = transform.GetEulerAngles();

		// Camera rotation
		if (Input::IsKeyPressed(MH_KEY_LEFT))
		{
			eulerAngles.y += rotationSpeed;
			transform.SetEulerangles(eulerAngles);
		}
		else if (Input::IsKeyPressed(MH_KEY_RIGHT))
		{
			eulerAngles.y -= rotationSpeed;
			transform.SetEulerangles(eulerAngles);
		}

		if (Input::IsKeyPressed(MH_KEY_UP))
		{
			eulerAngles.x += rotationSpeed;
			transform.SetEulerangles(eulerAngles);
		}
		else if (Input::IsKeyPressed(MH_KEY_DOWN))
		{
			eulerAngles.x -= rotationSpeed;
			transform.SetEulerangles(eulerAngles);
		}

		// Camera movement
		if (Input::IsKeyPressed(MH_KEY_A))
			transform.SetPosition(transform.GetPosition() - glm::vec3(speed) * transform.GetRight());
		else if (Input::IsKeyPressed(MH_KEY_D))
			transform.SetPosition(transform.GetPosition() + glm::vec3(speed) * transform.GetRight());

		if (Input::IsKeyPressed(MH_KEY_W))
			transform.SetPosition(transform.GetPosition() - glm::vec3(speed) * transform.GetForward());
		else if (Input::IsKeyPressed(MH_KEY_S))
			transform.SetPosition(transform.GetPosition() + glm::vec3(speed) * transform.GetForward());

		if (Input::IsKeyPressed(MH_KEY_Q))
			transform.SetPosition(transform.GetPosition() - glm::vec3(speed) * transform.GetUp());
		else if (Input::IsKeyPressed(MH_KEY_E))
			transform.SetPosition(transform.GetPosition() + glm::vec3(speed) * transform.GetUp());
	});
}

EXTERN_EXPORTED void Stop(Scene* scene)
{
	// TODO: Remove entities again
}

EXTERN_EXPORTED void Unload()
{
	MH_CORE_TRACE("DLL Unloaded!");

	ComponentRegistry::DeregisterComponent("Rotator");

	// TODO: Remove components from scene list of serializable components
	// TODO: Remove components from editor list for inspector
}