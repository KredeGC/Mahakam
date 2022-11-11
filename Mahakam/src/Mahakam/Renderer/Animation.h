#pragma once

#include "Mahakam/Core/Core.h"

#include "Mahakam/Asset/Asset.h"

#include <filesystem>
#include <string>

namespace Mahakam
{
	class Animation
	{
	private:
		struct Sampler
		{
			TrivialArray<float> Timestamps;

			UnorderedMap<int, TrivialArray<glm::vec3>> Translations;
			UnorderedMap<int, TrivialArray<glm::quat>> Rotations;
			UnorderedMap<int, TrivialArray<glm::vec3>> Scales;
		};

		std::filesystem::path m_Filepath;
		std::string m_Name;

		int m_AnimationIndex;
		float m_Duration = 0.0f;

		std::vector<Sampler> m_Samplers;

	public:
		Animation(const std::filesystem::path& filepath, int index);

		const std::string& GetName() const { return m_Name; }
		float GetDuration() const { return m_Duration; }

		const std::vector<Sampler>& GetSamplers() const { return m_Samplers; }

		inline static Asset<Animation> Load(const std::filesystem::path& filepath, int index = 0) { return LoadImpl(filepath, index); }

	private:
		MH_DECLARE_FUNC(LoadImpl, Asset<Animation>, const std::filesystem::path& filepath, int index);
	};
}