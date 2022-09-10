#pragma once

#include "Mahakam/Asset/Asset.h"

#include <filesystem>
#include <string>

namespace Mahakam
{
	class Animation;

	extern template class Asset<Animation>;

	class Animation
	{
	private:
		struct AnimPos
		{
			float Time;
			glm::vec3 Translation;
		};

		std::filesystem::path m_Filepath;
		std::string m_Name;

		int m_AnimationIndex;

		// TODO: Use darrays instead
		std::vector<float> m_Timestamps;

		UnorderedMap<int, std::vector<glm::vec3>> m_Translations;
		UnorderedMap<int, std::vector<glm::quat>> m_Rotations;
		UnorderedMap<int, std::vector<glm::vec3>> m_Scales;

	public:
		Animation(const std::filesystem::path& filepath, int index);

		const std::string& GetName() const { return m_Name; }
		float GetDuration() const { return *m_Timestamps.crbegin(); }

		const std::vector<float>& GetTimestamps() const { return m_Timestamps; }

		const UnorderedMap<int, std::vector<glm::vec3>>& GetTranslations() const { return m_Translations; }
		const UnorderedMap<int, std::vector<glm::quat>>& GetRotations() const { return m_Rotations; }
		const UnorderedMap<int, std::vector<glm::vec3>>& GetScales() const { return m_Scales; }

		inline static Asset<Animation> Load(const std::filesystem::path& filepath, int index = 0) { return LoadImpl(filepath, index); }

	private:
		MH_DECLARE_FUNC(LoadImpl, Asset<Animation>, const std::filesystem::path& filepath, int index);
	};
}