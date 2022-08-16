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
				Asset<Material> skyboxMaterial = Asset<Material>(skyboxPath);
				scene->SetSkyboxMaterial(skyboxMaterial);
			}

			std::filesystem::path irradiancePath = scene->GetSkyboxIrradiance().GetImportPath();
			if (GUI::DrawDragDropField("Skybox irradiance", ".texture", irradiancePath))
			{
				Asset<TextureCube> skyboxIrradiance = Asset<TextureCube>(irradiancePath);
				scene->SetSkyboxIrradiance(skyboxIrradiance);
			}

			std::filesystem::path specularPath = scene->GetSkyboxSpecular().GetImportPath();
			if (GUI::DrawDragDropField("Skybox specular", ".texture", specularPath))
			{
				Asset<TextureCube> skyboxSpecular = Asset<TextureCube>(specularPath);
				scene->SetSkyboxSpecular(skyboxSpecular);
			}
		}

		ImGui::End();
	}
}