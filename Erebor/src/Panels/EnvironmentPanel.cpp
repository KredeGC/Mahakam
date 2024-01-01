#include "ebpch.h"
#include "EnvironmentPanel.h"

namespace Mahakam::Editor
{
	void EnvironmentPanel::OnImGuiRender()
	{
		if (ImGui::Begin("Scene Environment", &m_Open))
		{
			Ref<Scene> scene = SceneManager::GetActiveScene();

			std::filesystem::path skyboxPath = scene->GetSkyboxMaterial().GetImportPath();
			if (GUI::DrawDragDropField("Skybox material", ".material", skyboxPath))
			{
				Asset<Material> skyboxMaterial = Asset<Material>(ResourceRegistry::GetImportInfo(skyboxPath).ID);
				scene->SetSkyboxMaterial(skyboxMaterial);
			}

			std::filesystem::path irradiancePath = scene->GetSkyboxIrradiance().GetImportPath();
			if (GUI::DrawDragDropField("Skybox irradiance", ".texture", irradiancePath))
			{
				Asset<TextureCube> skyboxIrradiance = Asset<TextureCube>(ResourceRegistry::GetImportInfo(irradiancePath).ID);
				scene->SetSkyboxIrradiance(skyboxIrradiance);
			}

			std::filesystem::path specularPath = scene->GetSkyboxSpecular().GetImportPath();
			if (GUI::DrawDragDropField("Skybox specular", ".texture", specularPath))
			{
				Asset<TextureCube> skyboxSpecular = Asset<TextureCube>(ResourceRegistry::GetImportInfo(specularPath).ID);
				scene->SetSkyboxSpecular(skyboxSpecular);
			}
		}

		ImGui::End();
	}
}