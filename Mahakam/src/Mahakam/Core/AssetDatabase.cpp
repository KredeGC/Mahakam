#include "mhpch.h"
#include "AssetDatabase.h"

namespace Mahakam
{
	void AssetDatabase::RegisterAssetImporter(const std::string& extension, Ref<AssetImporter> assetImport)
	{
		assetInterfaces[extension] = assetImport;
	}

	void AssetDatabase::DeregisterAssetImporter(const std::string& extension)
	{
		assetInterfaces.erase(extension);
	}

	void AssetDatabase::SaveAsset(Ref<void> asset, const std::string& extension, const std::filesystem::path& filepath)
	{
		auto iter = assetInterfaces.find(extension);
		if (iter != assetInterfaces.end())
		{
			YAML::Emitter emitter;
			emitter << YAML::BeginMap;

			iter->second->Serialize(emitter, asset);

			emitter << YAML::EndMap;

			std::ofstream filestream(filepath);
			filestream << emitter.c_str();
		}
	}

	Ref<void> AssetDatabase::LoadAsset(const std::filesystem::path& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath.string());
		}
		catch (YAML::ParserException e)
		{
			MH_CORE_BREAK(e.msg);
		}

		
	}
}