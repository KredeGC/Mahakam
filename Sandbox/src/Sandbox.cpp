#include "sbpch.h"

#include <Mahakam.h>

#if defined _WIN32 || defined __CYGWIN__
#ifdef WIN_EXPORT
#ifdef __GNUC__
#define EXPORTED __attribute__ ((dllexport))
#else
#define EXPORTED __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#endif
#else
#ifdef __GNUC__
#define EXPORTED __attribute__ ((dllimport))
#else
#define EXPORTED __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#else
#if __GNUC__ >= 4
#define EXPORTED __attribute__ ((visibility ("default")))
#else
#define EXPORTED
#endif
#endif

#define EXTERN_EXPORTED extern "C" EXPORTED

using namespace Mahakam;

static RenderingContext* context;

struct EXPORTED TestComponent
{
	int h;

public:
	TestComponent(int h)
		: h(h) {}
};

EXTERN_EXPORTED void Init(Application* application, Scene* scene)
{
	Log::Init();

    context = RenderingContext::Create(application->GetWindow().GetNativeWindow(), application->GetWindow().GetProcess());
    context->Reload();


	// TODO: Add components to list of components


	Ref<Shader> skinnedShader = Shader::Create("assets/shaders/default/Skinned.yaml");
	Ref<Material> skinnedMaterial = Material::Create(skinnedShader);
	skinnedMaterial->SetFloat3("u_Color", { 0.68f, 0.44f, 0.22f });
	skinnedMaterial->SetFloat("u_Metallic", 1.0f);
	skinnedMaterial->SetFloat("u_Roughness", 0.4f);

	SkinnedMesh testSkinnedModel = AssetDatabase::CreateOrLoadAsset<SkinnedMesh>("assets/models/Defeated.fbx");

	SkinnedMesh skinnedModel = Mesh::LoadModel("assets/models/Defeated.fbx");
	Ref<Animation> animation = Animation::load("assets/models/Defeated.fbx", skinnedModel);

	Entity animatedEntity = scene->CreateEntity("Animated");
	animatedEntity.AddComponent<MeshComponent>(skinnedModel, skinnedMaterial);
	animatedEntity.GetComponent<TransformComponent>().SetPosition({ 4.5f, 1.5f, 5.0f });
	animatedEntity.GetComponent<TransformComponent>().SetScale({ 0.02f, 0.02f, 0.02f });
	animatedEntity.AddComponent<AnimatorComponent>(animation);
	animatedEntity.AddComponent<TestComponent>(4);



	Ref<Shader> colorShader = Shader::Create("assets/shaders/default/LitColor.yaml");
	Ref<Mesh> sphereMesh = Mesh::CreateCubeSphere(9);
	Ref<Material> material = Material::Create(colorShader);
	material->SetFloat3("u_Color", { 1.0f, 1.0f, 1.0f });

	// Setup material with texture
	material->SetFloat("u_Metallic", 0.0f);
	material->SetFloat("u_Roughness", 0.0f);

	// Create entity
	Entity entity = scene->CreateEntity("Sphere Test");
	entity.AddComponent<MeshComponent>(sphereMesh, material);
	entity.GetComponent<TransformComponent>().SetPosition({ 0.0f, 0.0f, 1.0f });
}

EXTERN_EXPORTED void Shutdown(Scene* scene, Timestep dt)
{
	// TODO: Remove components from list of components
}

EXTERN_EXPORTED void Update(Scene* scene, Timestep dt)
{

}