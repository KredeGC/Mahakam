#include "ebpch.h"
#include "ImportWizardPanel.h"

namespace Mahakam::Editor
{
	static ryml::Tree CreateEmptyTree()
	{
		ryml::Tree tree;

		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;

		return tree;
	}

	static ryml::Tree ImportTree(ResourceImporter& importer)
	{
		ryml::Tree tree;

		ryml::NodeRef root = tree.rootref();
		root |= ryml::MAP;

		importer.OnImport(root);

		return tree;
	}

	static ryml::Tree ReadImport(const std::filesystem::path& importPath)
	{
		if (!FileUtility::Exists(importPath))
			return CreateEmptyTree();

		TrivialVector<char> buffer;

		if (!FileUtility::ReadFile(importPath, buffer))
			return CreateEmptyTree();

		try
		{
			return ryml::parse_in_arena(ryml::csubstr(buffer.data(), buffer.size()));
		}
		catch (std::runtime_error const& e)
		{
			MH_WARN("Asset Import Wizard experienced an error when reading {0}: {1}", importPath.string(), e.what());

			return CreateEmptyTree();
		}
	}

	static void SaveImport(ryml::Tree& tree, const std::filesystem::path& importPath, ResourceImporter& importer, AssetDatabase::AssetID id)
	{
		MH_ASSERT(id, "AssetID cannot be 0");

		ryml::NodeRef root = tree.rootref();

		// Create asset from tree
		Asset<void> asset = importer.CreateAsset(root);

		// Add ID and extension to tree root
		root["Extension"] << importer.GetImporterProps().Extension;
		root["ID"] << id;

		// Save tree to file
		std::ofstream filestream(importPath);
		filestream << tree;
		filestream.close();

		// Save asset
		asset.Save(id, importer.GetImporterProps().Extension);
	}

	void ImportWizardPanel::OnImGuiRender()
	{
		Ref<ResourceImporter> importer;
		if (m_Open && (importer = m_Importer.lock()))
		{
			if (ImGui::Begin("Import Asset", &m_Open))
			{
				importer->OnRender();

				ImGui::Separator();

				// TODO: Draw AssetID so it can be easily changed
				// Could also warn if an asset already exists with that ID
				int step = 1;
				int step_fast = 100;
				ImGui::InputScalar("ID", ImGuiDataType_U64, &m_AssetID, nullptr, nullptr, "%llu", 0);

				if (ImGui::Button("Regenerate ID"))
					m_AssetID = Random::GetRandomID64();

				//if (AssetDatabase::AssetExists(m_AssetID))
				//	ImGui::Text("An asset with this ID already exists");

				// Draw filename input without path/**.extension.import
				std::string nameString = m_ImportPath.stem().stem().string();
				char nameBuffer[GUI::MAX_STR_LEN]{ 0 };
				strncpy(nameBuffer, nameString.c_str(), nameString.size());
				if (ImGui::InputText("Import filename", nameBuffer, GUI::MAX_STR_LEN))
				{
					m_ImportPath = m_ImportPath.parent_path() / (std::string(nameBuffer) + importer->GetImporterProps().Extension + ".import");
				}

				// Draw import path input without the .extension.import
				std::string importString = (m_ImportPath.parent_path() / m_ImportPath.stem().stem()).string();
				char importBuffer[GUI::MAX_STR_LEN]{ 0 };
				strncpy(importBuffer, importString.c_str(), importString.size());
				if (ImGui::InputText("Import path", importBuffer, GUI::MAX_STR_LEN))
				{
					m_ImportPath = std::string(importBuffer) + importer->GetImporterProps().Extension + ".import";
				}

				// Draw import button
				if (ImGui::Button("Import"))
				{
					// Import tree
					ryml::Tree tree = ImportTree(*importer);

					// Save tree
					SaveImport(tree, m_ImportPath, *importer, m_AssetID);

					m_Open = false;
				}
			}

			ImGui::End();
		}
	}

	void ImportWizardPanel::ResourceOpen(const std::filesystem::path& filepath, const std::string& extension)
	{
		if (Ref<ResourceImporter> importer = ResourceRegistry::GetAssetImporter(extension))
		{
			if (importer->GetImporterProps().NoWizard)
			{
				// TODO: Call OnResourceOpen

				// Import tree
				ryml::Tree tree = ImportTree(*importer);

				// Save tree
				std::filesystem::path importPath = FileUtility::GetImportPath(filepath, importer->GetImporterProps().Extension);
				SaveImport(tree, importPath, *importer, Random::GetRandomID64());
			}
			else
			{
				ImportWizardPanel* window = (ImportWizardPanel*)EditorWindowRegistry::OpenWindow("Import Wizard");

				window->m_Open = true;
				window->m_Importer = importer;
				window->m_ImportPath = FileUtility::GetImportPath(filepath, importer->GetImporterProps().Extension);
				window->m_AssetID = Random::GetRandomID64();

				// Open the wizard with the filepath
				importer->OnResourceOpen(filepath);
			}
		}
	}

	void ImportWizardPanel::ImportOpen(const std::filesystem::path& importPath, const std::string& extension)
	{
		if (Ref<ResourceImporter> importer = ResourceRegistry::GetAssetImporter(extension))
		{
			if (importer->GetImporterProps().NoWizard)
			{
				// Read tree
				ryml::Tree tree = ReadImport(importPath);

				ryml::NodeRef root = tree.rootref();

				AssetDatabase::AssetID assetID;
				DeserializeYAMLNode(root, "ID", assetID);

				importer->OnImportOpen(root);

				importer->OnImport(root);

				// Save tree
				SaveImport(tree, importPath, *importer, assetID);
			}
			else
			{
				ImportWizardPanel* window = (ImportWizardPanel*)EditorWindowRegistry::OpenWindow("Import Wizard");

				window->m_Open = true;
				window->m_Importer = importer;
				window->m_ImportPath = importPath;
				window->m_AssetID = Random::GetRandomID64();

				// Read the import file and open the wizard
				ryml::Tree tree = ReadImport(importPath);

				ryml::NodeRef root = tree.rootref();

				DeserializeYAMLNode(root, "ID", window->m_AssetID);

				importer->OnImportOpen(root);
			}
		}
	}
}