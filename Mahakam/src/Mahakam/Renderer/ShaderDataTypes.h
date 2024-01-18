#pragma once

#include "Mahakam/Core/Allocator.h"
#include "Mahakam/Core/Types.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace Mahakam
{
	enum class ShaderStage
	{
		None = 0,
		Vertex,
		Fragment
	};

	enum class ShaderDataType
	{
		None = 0,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool,
		Sampler2D,
		SamplerCube
	};

	struct ShaderSource
	{
		UnorderedMap<ShaderStage, std::string> Sources;
		std::string Defines;
	};

	class ShaderData
	{
	public:
		ShaderData() :
			m_ShaderData(Allocator::GetAllocator<uint32_t>()),
			m_Offsets(Allocator::GetAllocator<std::pair<const ShaderStage, std::pair<size_t, size_t>>>()) {}

		ShaderData(const ShaderData&) = delete;
		ShaderData(ShaderData&&) noexcept = default;

		ShaderData& operator=(const ShaderData&) = delete;
		ShaderData& operator=(ShaderData&&) noexcept = default;

		void SetStage(ShaderStage index, const uint32_t* data, size_t size)
		{
			auto iter = m_Offsets.find(index);
			if (iter != m_Offsets.end())
			{
				MH_BREAK("Updating an existing ShaderData is not currently supported");
				return;
			}

			m_Offsets[index] = std::make_pair(m_ShaderData.size(), size);
			m_ShaderData.push_back(data, data + size);
		}

		template<typename T>
		void SetStage(ShaderStage index, const T& container)
		{
			SetStage(index, container.data(), container.size());
		}

		std::pair<const uint32_t*, size_t> GetStage(ShaderStage index) const
		{
			auto iter = m_Offsets.find(index);
			if (iter == m_Offsets.end())
				return std::make_pair(nullptr, 0);

			return std::make_pair(m_ShaderData.data() + iter->second.first, iter->second.second);
		}

		const auto& GetShaderData() const { return m_ShaderData; }
		const auto& GetOffsets() const { return m_Offsets; }
		size_t GetStageCount() const noexcept { return m_Offsets.size(); }

		explicit operator bool() const noexcept { return !m_Offsets.empty(); }

	private:
		TrivialVector<uint32_t, Allocator::BaseAllocator<uint32_t>> m_ShaderData;
		UnorderedMap<ShaderStage, std::pair<size_t, size_t>, Allocator::BaseAllocator<std::pair<const ShaderStage, std::pair<size_t, size_t>>>> m_Offsets;
	};


	// Mostly editor stuff?

	enum class ShaderPropertyType
	{
		Color,
		HDR,
		Vector,
		Range,
		Drag,
		Texture,
		Normal,
		Default
	};

	struct ShaderProperty
	{
		ShaderPropertyType PropertyType; // Editor
		ShaderDataType DataType;
		float Min = 0; // Editor
		float Max = 0; // Editor
		std::string DefaultString; // Editor-ish?
		uint32_t Count = 1;
		int Offset = 0;
	};

	// TODO: Deprecated
	using SourceDefinition = ShaderSource;
}