#pragma once

#include "Mahakam/Core/Allocator.h"
#include "Mahakam/Core/Types.h"

#include "Mahakam/Asset/Asset.h"

#include <filesystem>
#include <string>

namespace Mahakam
{
	class Animation
	{
	public:
		struct Sampler
		{
			TrivialArray<float> Timestamps;

			UnorderedMap<int, TrivialArray<glm::vec3>> Translations;
			UnorderedMap<int, TrivialArray<glm::quat>> Rotations;
			UnorderedMap<int, TrivialArray<glm::vec3>> Scales;
		};

		using SamplerType = std::vector<Sampler, Allocator::BaseAllocator<Sampler>>;

	private:
		std::string m_Name;

		float m_Duration = 0.0f;

		SamplerType m_Samplers;

	public:
		Animation(const std::filesystem::path& filepath, int index);
		Animation(const std::string& name, SamplerType&& samplers, float duration);

		inline const std::string& GetName() const { return m_Name; }
		inline float GetDuration() const { return m_Duration; }

		inline const SamplerType& GetSamplers() const { return m_Samplers; }

		inline static Asset<Animation> Load(const std::filesystem::path& filepath, int index = 0) { return CreateAsset<Animation>(filepath, index); }
		inline static Asset<Animation> Create(const std::string& name, SamplerType&& samplers, float duration) { return CreateAsset<Animation>(name, std::move(samplers), duration); }
	};
}