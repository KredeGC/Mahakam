#include "Mahakam/mhpch.h"
#include "Animation.h"

#include "Mahakam/Core/Allocator.h"
#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Profiler.h"
#include "Mahakam/Core/SharedLibrary.h"

#include <tiny_gltf/tiny_gltf.h>

namespace Mahakam
{
	Animation::Animation(const std::filesystem::path& filepath, int index)
		: m_Filepath(filepath), m_AnimationIndex(index)
	{
		MH_PROFILE_FUNCTION();

		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		bool success;
		if (filepath.extension().string() == ".gltf")
			success = loader.LoadASCIIFromFile(&model, &err, &warn, filepath.string());
		else
			success = loader.LoadBinaryFromFile(&model, &err, &warn, filepath.string());

		if (!warn.empty())
			MH_CORE_WARN("[GLTF] Warning: {0}", warn);

		if (!err.empty())
			MH_CORE_ERROR("[GLTF] Error: {0}", err);

		if (!success) {
			MH_CORE_ERROR("[GLTF] Failed to parse glTF model at {0}", filepath.string());
			return;
		}

		auto& animations = model.animations;
		auto& animation = animations[m_AnimationIndex];
		m_Name = animation.name;

		UnorderedMap<int, Sampler> samplers;

		// Each channel refers to a node/bone's target
		// channels.size() ~= bones.size() * [translation, rotation, scale, weights].size()
		for (auto& channel : animation.channels)
		{
			auto& sampler = animation.samplers[channel.sampler];

			// Input data (timestamps)
			if (samplers.find(sampler.input) == samplers.end())
			{
				const auto& inputAccessor = model.accessors[sampler.input];
				const auto& inputBufferView = model.bufferViews[inputAccessor.bufferView];
				const auto& inputBuffer = model.buffers[inputBufferView.buffer];

				const float* times = reinterpret_cast<const float*>(&inputBuffer.data[inputBufferView.byteOffset + inputAccessor.byteOffset]);

				float duration = times[inputAccessor.count - 1];
				if (duration > m_Duration)
					m_Duration = duration;

				samplers[sampler.input].Timestamps.assign(times, times + inputAccessor.count);
			}

			// Output data (the actual values)
			const auto& outputAccessor = model.accessors[sampler.output];
			const auto& outputBufferView = model.bufferViews[outputAccessor.bufferView];
			const auto& outputBuffer = model.buffers[outputBufferView.buffer];

			//MH_CORE_ASSERT(outputAccessor.count == inputAccessor.count, "The number of keyframes and sampler values must match");

			if (channel.target_path == "translation")
			{
				// Extract translations from the buffer
				const glm::vec3* values = reinterpret_cast<const glm::vec3*>(&outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset]);

				samplers[sampler.input].Translations[channel.target_node].assign(values, values + outputAccessor.count);
			}
			else if (channel.target_path == "rotation")
			{
				// Extract rotations from the buffer
				const glm::quat* values = reinterpret_cast<const glm::quat*>(&outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset]);

				samplers[sampler.input].Rotations[channel.target_node].assign(values, values + outputAccessor.count);
			}
			else if (channel.target_path == "scale")
			{
				// Extract scales from the buffer
				const glm::vec3* values = reinterpret_cast<const glm::vec3*>(&outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset]);

				samplers[sampler.input].Scales[channel.target_node].assign(values, values + outputAccessor.count);
			}
			else if (channel.target_path == "weights")
			{
				MH_CORE_BREAK("Weight animations are not yet supported");
			}
		}

		m_Samplers.reserve(samplers.size());
		for (auto& kv : samplers)
			m_Samplers.push_back(kv.second);
	}

	//Asset<Animation> Animation::LoadImpl(const std::filesystem::path& filepath)
	MH_DEFINE_FUNC(Animation::LoadImpl, Asset<Animation>, const std::filesystem::path& filepath, int index)
	{
		return CreateAsset<Animation>(filepath, index);

		/*Animation* animation = Allocator::Allocate<Animation>(1);
		Allocator::Construct<Animation>(animation, filepath, index);

		auto deleter = [](void* p)
		{
			Allocator::Deconstruct<Animation>(static_cast<Animation*>(p));
			Allocator::Deallocate<Animation>(static_cast<Animation*>(p), 1);
		};

		return Asset<Animation>(animation, deleter);*/
	};
}