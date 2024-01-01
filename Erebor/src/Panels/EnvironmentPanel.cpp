#include "ebpch.h"
#include "EnvironmentPanel.h"

namespace Mahakam::Editor
{
	void EnvironmentPanel::OnImGuiRender()
	{
		if (ImGui::Begin("Scene Environment", &m_Open))
		{
			Ref<Scene> scene = SceneManager::GetActiveScene();

			Asset<Material> skyboxMaterial = scene->GetSkyboxMaterial();
			if (GUI::DrawDragDropAsset("Skybox material", skyboxMaterial, ".material"))
				scene->SetSkyboxMaterial(skyboxMaterial);

			Asset<TextureCube> skyboxIrradiance = scene->GetSkyboxIrradiance();
			if (GUI::DrawDragDropAsset("Skybox irradiance", skyboxIrradiance, ".texcube"))
				scene->SetSkyboxIrradiance(skyboxIrradiance);

			Asset<TextureCube> skyboxSpecular = scene->GetSkyboxSpecular();
			if (GUI::DrawDragDropAsset("Skybox specular", skyboxSpecular, ".texcube"))
				scene->SetSkyboxSpecular(skyboxSpecular);
		}

		ImGui::End();
	}
}