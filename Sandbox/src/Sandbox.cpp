#include "sbpch.h"

#include <Mahakam.h>
#include <MahakamRuntime.h>

using namespace Mahakam;

struct EXPORTED TestComponent
{
	float rotation;

public:
	TestComponent()
		: rotation(0.0f) {}
};

EXTERN_EXPORTED void Load(void** funcPtrs)
{
	SharedLibrary::ImportFuncPointers(funcPtrs);

	Ref<Texture2D> testTex = Texture2D::Create("assets/textures/brick/brick_albedo.png", { TextureFormat::SRGB_DXT1, TextureFilter::Trilinear });
	testTex->GetRendererID();

	MH_CORE_TRACE("DLL Loaded!");


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
		for (int y = 0; y < 2; y++)
		{
			Entity animatedEntity = scene->CreateEntity("DLL Animated");
			animatedEntity.AddComponent<MeshComponent>(skinnedModel, skinnedMaterial);
			animatedEntity.GetComponent<TransformComponent>().SetPosition({ x * 5.0f, 1.5f, 5.0f + y * 5.0f });
			animatedEntity.GetComponent<TransformComponent>().SetScale({ 0.02f, 0.02f, 0.02f });
			animatedEntity.AddComponent<AnimatorComponent>(animation);
		}
	}


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
	backpackEntity.AddComponent<TestComponent>();


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

	scene->ForEach<TransformComponent, TestComponent>([&](auto entity, TransformComponent& transform, TestComponent& testComponent)
	{
		testComponent.rotation += dt * 10.0f;
		transform.SetRotation(glm::quat(glm::vec3{ 0.0f, glm::radians(testComponent.rotation), 0.0f }));
	});
}

EXTERN_EXPORTED void Stop(Scene* scene)
{
	// TODO: Remove entities again
}

EXTERN_EXPORTED void Unload()
{
	MH_CORE_TRACE("DLL Unloaded!");



	// TODO: Remove components from scene list of serializable components
	// TODO: Remove components from editor list for inspector
}