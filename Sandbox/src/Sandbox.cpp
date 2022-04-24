#include "sbpch.h"

#include <Mahakam.h>
#include <MahakamRuntime.h>

using namespace Mahakam;

struct EXPORTED RotatorComponent
{
	float rotation = 0.0f;;
	float rotationSpeed = 10.0f;

public:
	RotatorComponent() {}
};

class CameraController : public ScriptableEntity
{
public:
	virtual void OnUpdate(Timestep dt) override
	{
		auto& transform = GetComponent<TransformComponent>();

		float speed = 20.0f * dt;
		float rotationSpeed = dt;

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
	}
};

EXTERN_EXPORTED void Load(ImGuiContext* context, void** funcPtrs)
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

	Ref<Texture2D> testTex = Texture2D::Create("assets/textures/brick/brick_albedo.png", { TextureFormat::SRGB_DXT1, TextureFilter::Point });
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


	// Create shaders
	Ref<Shader> skinnedShader = Shader::Create("assets/shaders/default/Skinned.yaml");
	Ref<Shader> textureShader = Shader::Create("assets/shaders/default/LitTexture.yaml");
	Ref<Shader> colorShader = Shader::Create("assets/shaders/default/LitColor.yaml");
	/*Ref<Shader> skinnedShader = Shader::Create("assets/shaders/external/DitheredSkinned.yaml");
	Ref<Shader> textureShader = Shader::Create("assets/shaders/external/LitTexel.yaml");
	Ref<Shader> colorShader = Shader::Create("assets/shaders/external/DitheredColor.yaml");*/


	// Create skinned material
	Ref<Material> skinnedMaterial = Material::Create(skinnedShader);
	skinnedMaterial->SetFloat3("u_Color", { 0.68f, 0.44f, 0.22f });
	skinnedMaterial->SetFloat("u_Metallic", 1.0f);
	skinnedMaterial->SetFloat("u_Roughness", 0.4f);

	// Create animation and model
	SkinnedMesh skinnedModel = Mesh::LoadModel("assets/models/remy_twerk.fbx");
	Ref<Animation> animation = Animation::Load("assets/models/remy_twerk.fbx", skinnedModel);

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


	// Scene camera
	Entity cameraEntity = scene->CreateEntity("Main Camera");
	cameraEntity.AddComponent<CameraComponent>(Camera::ProjectionType::Perspective, glm::radians(45.0f), 0.01f, 100.0f);
	cameraEntity.GetComponent<TransformComponent>().SetPosition({ 4.5f, 4.5f, 12.5f });


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
	Ref<Texture> brickAlbedo = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/brick/brick_albedo.png", false, { 128, 128, TextureFormat::SRGB_DXT1, TextureFilter::Point });
	Ref<Texture> brickBump = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/brick/brick_bump.png", false, { 128, 128, TextureFormat::RG_BC5, TextureFilter::Point });
	Ref<Texture> brickRoughness = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/brick/brick_roughness.png", false, { 128, 128, TextureFormat::R_BC4, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
	Ref<Mesh> planeMesh = Mesh::CreatePlane(2, 2);

	Ref<Material> planeMaterial = Material::Create(textureShader);
	planeMaterial->SetTexture("u_Albedo", 0, brickAlbedo); // TODO: Fix this
	planeMaterial->SetTexture("u_Bump", 0, GL::GetTexture2DBump());
	planeMaterial->SetTexture("u_Metallic", 0, GL::GetTexture2DBlack());
	planeMaterial->SetTexture("u_Roughness", 0, brickRoughness);

	Entity planeEntity = scene->CreateEntity("Plane");
	planeEntity.AddComponent<MeshComponent>(planeMesh, planeMaterial);
	planeEntity.GetComponent<TransformComponent>().SetPosition({ 0.0f, -1.0f, 0.0f });
	planeEntity.GetComponent<TransformComponent>().SetScale({ 30.0f, 30.0f, 30.0f });


	// Create particle system
	Entity particleEntity = scene->CreateEntity("Particle System");
	particleEntity.AddComponent<ParticleSystemComponent>();
	particleEntity.GetComponent<TransformComponent>().SetPosition({ 0.0f, 0.0f, 1.0f });


	// Create backpack textures
	Ref<Texture2D> backpackDiffuse = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/diffuse.jpg", false, { 4096, 4096, TextureFormat::SRGB_DXT1 });
	Ref<Texture> backpackOcclussion = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/ao.jpg", false, { 4096, 4096, TextureFormat::R_BC4 });
	Ref<Texture> backpackBump = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/normal.png", false, { 4096, 4096, TextureFormat::RG_BC5 });
	Ref<Texture> backpackMetallic = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/specular.jpg", false, { 4096, 4096, TextureFormat::R_BC4 });
	Ref<Texture> backpackRoughness = AssetDatabase::CreateOrLoadAsset<Texture2D>("assets/textures/backpack/roughness.jpg", false, { 4096, 4096, TextureFormat::R_BC4 });

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


	// Create mesh & base material
	Ref<Mesh> sphereMesh = Mesh::CreateCubeSphere(9);
	Ref<Material> baseMaterial = Material::Create(colorShader);
	baseMaterial->SetFloat3("u_Color", { 1.0f, 1.0f, 1.0f });

	// Create scene entities
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			// Setup material with texture
			Ref<Material> material = Material::Copy(baseMaterial);
			material->SetFloat("u_Metallic", y / 10.0f);
			material->SetFloat("u_Roughness", x / 10.0f);

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

	scene->ForEach<TransformComponent, RotatorComponent>([&](auto entity, TransformComponent& transform, RotatorComponent& rotator)
	{
		rotator.rotation += dt * rotator.rotationSpeed;
		transform.SetRotation(glm::quat(glm::vec3{ 0.0f, glm::radians(rotator.rotation), 0.0f }));
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