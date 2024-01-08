#pragma once

#include "ShaderDataTypes.h"

#include "Mahakam/Asset/Asset.h"

#define GLM_FORCE_INLINE
#define GLM_FORCE_INTRINSICS
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

#include <filesystem>
#include <string>

namespace Mahakam
{
	class Texture;

	class ShaderData
	{
	public:
		ShaderData() :
			m_ShaderData(Allocator::GetAllocator<uint32_t>()),
			m_Offsets(Allocator::GetAllocator<std::pair<const ShaderStage, size_t>>()) {}

		ShaderData(const ShaderData&) = delete;

		ShaderData(ShaderData&&) noexcept = default;

		void SetVertices(ShaderStage index, const uint32_t* data, size_t size)
		{
			auto iter = m_Offsets.find(index);
			if (iter != m_Offsets.end())
			{
				MH_BREAK("Updating an existing ShaderData is not currently supported");
				return;
			}

			m_Offsets[index] = m_ShaderData.size();
			m_ShaderData.push_back(data, data + size);
		}

		template<typename T>
		void SetVertices(ShaderStage index, const T& container)
		{
			SetVertices(index, container.data(), container.size());
		}

		const auto& GetShaderData() const { return m_ShaderData; }
		const auto& GetOffsets() const { return m_Offsets; }

	private:
		TrivialVector<uint32_t, Allocator::BaseAllocator<uint32_t>> m_ShaderData;
		UnorderedMap<ShaderStage, size_t, Allocator::BaseAllocator<std::pair<const ShaderStage, size_t>>> m_Offsets;
	};

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind(const std::string& shaderPass) = 0;

		virtual const std::filesystem::path& GetFilepath() const = 0;
		virtual const std::string& GetName() const = 0;

		//virtual const UnorderedMap<std::string, UnorderedMap<ShaderStage, std::vector<uint32_t>>>& GetData() const = 0;
		virtual const UnorderedMap<std::string, ShaderProperty>& GetProperties() const = 0;

		virtual bool HasShaderPass(const std::string& shaderPass) const = 0;

		virtual void SetTexture(const std::string& name, Texture& tex) = 0;

		virtual void SetUniformMat3(const std::string& name, const glm::mat3& value) = 0;
		virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) = 0;
		 
		virtual void SetUniformInt(const std::string& name, int value) = 0;
		 
		virtual void SetUniformFloat(const std::string& name, float value) = 0;
		virtual void SetUniformFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetUniformFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetUniformFloat4(const std::string& name, const glm::vec4& value) = 0;

		inline static Asset<Shader> Create(const std::filesystem::path& filepath) { return CreateFilepath(filepath); }

	private:
		MH_DECLARE_FUNC(CreateFilepath, Asset<Shader>, const std::filesystem::path& filepath);
	};
}