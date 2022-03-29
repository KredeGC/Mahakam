#include "sbpch.h"

#include <Mahakam.h>
#include <MahakamExport.h>

using namespace Mahakam;

struct EXPORTED TestComponent
{
	int h;

public:
	TestComponent(int h)
		: h(h) {}
};

EXTERN_EXPORTED void Load(void** funcPtrs)
{
	SharedLibrary::ImportFuncPointers(funcPtrs);

	Ref<Texture2D> testTex = Texture2D::Create("assets/textures/brick/brick_albedo.png", { TextureFormat::SRGB_DXT1, TextureFilter::Point });
	testTex->GetRendererID();


	// TODO: Add components to list of components
}

EXTERN_EXPORTED void Run(Scene* scene)
{
	// Create entity
	Entity entity = scene->CreateEntity("DLL Light Test");
	entity.AddComponent<LightComponent>(Light::LightType::Spot, glm::radians(45.0f), 10.0f, glm::vec3(1.0f, 1.0f, 1.0f), true);
	entity.GetComponent<TransformComponent>().SetPosition({ 0.0f, 0.0f, 1.0f });



	//Ref<Shader> skinnedShader = Shader::Create("assets/shaders/default/Skinned.yaml");
	//Ref<Material> skinnedMaterial = Material::Create(skinnedShader);
	//skinnedMaterial->SetFloat3("u_Color", { 0.68f, 0.44f, 0.22f });
	//skinnedMaterial->SetFloat("u_Metallic", 1.0f);
	//skinnedMaterial->SetFloat("u_Roughness", 0.4f);

	//SkinnedMesh testSkinnedModel = AssetDatabase::CreateOrLoadAsset<SkinnedMesh>("assets/models/Defeated.fbx");

	//SkinnedMesh skinnedModel = Mesh::LoadModel("assets/models/Defeated.fbx");
	//Ref<Animation> animation = Animation::load("assets/models/Defeated.fbx", skinnedModel);

	//Entity animatedEntity = scene->CreateEntity("Animated");
	//animatedEntity.AddComponent<MeshComponent>(skinnedModel, skinnedMaterial);
	//animatedEntity.GetComponent<TransformComponent>().SetPosition({ 4.5f, 1.5f, 5.0f });
	//animatedEntity.GetComponent<TransformComponent>().SetScale({ 0.02f, 0.02f, 0.02f });
	//animatedEntity.AddComponent<AnimatorComponent>(animation);
	//animatedEntity.AddComponent<TestComponent>(4);



	//Ref<Shader> colorShader = Shader::Create("assets/shaders/default/LitColor.yaml");
	//Ref<Mesh> sphereMesh = Mesh::CreateCubeSphere(9);
	//Ref<Material> material = Material::Create(colorShader);
	//material->SetFloat3("u_Color", { 1.0f, 1.0f, 1.0f });

	//// Setup material with texture
	//material->SetFloat("u_Metallic", 0.0f);
	//material->SetFloat("u_Roughness", 0.0f);

	//// Create entity
	//Entity entity = scene->CreateEntity("Sphere Test");
	//entity.AddComponent<MeshComponent>(sphereMesh, material);
	//entity.GetComponent<TransformComponent>().SetPosition({ 0.0f, 0.0f, 1.0f });
}

EXTERN_EXPORTED void Update(Scene* scene, Timestep dt)
{

}

EXTERN_EXPORTED void Stop(Scene* scene)
{
	// TODO: Remove entities again
}

EXTERN_EXPORTED void Unload()
{
	// TODO: Remove components from list of components
}