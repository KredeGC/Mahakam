#pragma once

#include "AssetSerialization.h"
#include "ContainerSerialization.h"
#include "MathSerialization.h"
#include "ShaderSerialization.h"

#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/Shader.h"

#include <bitstream.h>

#define BS_ASSERT(...) if (!(__VA_ARGS__)) { return false; }

namespace bitstream
{
	template<>
	struct serialize_traits<Mahakam::Material>
	{
		template<auto F, typename Stream>
		static bool write_value(Stream& stream, const Mahakam::Material& material, const std::string& name) noexcept
		{
			return stream.serialize((material.*F)(name));
		}

		template<auto F, typename T, typename Stream>
		static bool read_value(Stream& stream, Mahakam::Material& material, const std::string& name) noexcept
		{
			T value;
			BS_ASSERT(stream.serialize(value));
			(material.*F)(name, value);

			return true;
		}

		template<typename Stream>
		static bool serialize(Stream& stream, inout<Stream, Mahakam::Asset<Mahakam::Material>> material) noexcept
		{
			using namespace Mahakam;

			if constexpr (Stream::writing)
			{
				BS_ASSERT(stream.serialize(material->GetShader()));

				auto& properties = material->GetShader()->GetProperties();
				for (auto& kv : properties)
				{
					switch (kv.second.DataType)
					{
					case ShaderDataType::Float:			BS_ASSERT(write_value<&Material::GetFloat>(stream, *material, kv.first)); break;
					case ShaderDataType::Float2:		BS_ASSERT(write_value<&Material::GetFloat2>(stream, *material, kv.first)); break;
					case ShaderDataType::Float3:		BS_ASSERT(write_value<&Material::GetFloat3>(stream, *material, kv.first)); break;
					case ShaderDataType::Float4:		BS_ASSERT(write_value<&Material::GetFloat4>(stream, *material, kv.first)); break;
					case ShaderDataType::Mat3:			BS_ASSERT(write_value<&Material::GetMat3>(stream, *material, kv.first)); break;
					case ShaderDataType::Mat4:			BS_ASSERT(write_value<&Material::GetMat4>(stream, *material, kv.first)); break;
					case ShaderDataType::Int:			BS_ASSERT(write_value<&Material::GetInt>(stream, *material, kv.first)); break;
					case ShaderDataType::Sampler2D:		BS_ASSERT(write_value<&Material::GetTexture>(stream, *material, kv.first)); break;
					case ShaderDataType::SamplerCube:	BS_ASSERT(write_value<&Material::GetTexture>(stream, *material, kv.first)); break;
					default: break;
					}
				}
			}
			else
			{
				Asset<Shader> shader;
				BS_ASSERT(stream.serialize(shader));

				material = Material::Create(shader);

				auto& properties = shader->GetProperties();
				for (auto& kv : properties)
				{
					switch (kv.second.DataType)
					{
					case ShaderDataType::Float:			BS_ASSERT(read_value<&Material::SetFloat, float>(stream, *material, kv.first)); break;
					case ShaderDataType::Float2:		BS_ASSERT(read_value<&Material::SetFloat2, glm::vec2>(stream, *material, kv.first)); break;
					case ShaderDataType::Float3:		BS_ASSERT(read_value<&Material::SetFloat3, glm::vec3>(stream, *material, kv.first)); break;
					case ShaderDataType::Float4:		BS_ASSERT(read_value<&Material::SetFloat4, glm::vec4>(stream, *material, kv.first)); break;
					case ShaderDataType::Mat3:			BS_ASSERT(read_value<&Material::SetMat3, glm::mat3>(stream, *material, kv.first)); break;
					case ShaderDataType::Mat4:			BS_ASSERT(read_value<&Material::SetMat4, glm::mat4>(stream, *material, kv.first)); break;
					case ShaderDataType::Int:			BS_ASSERT(read_value<&Material::SetInt, int32_t>(stream, *material, kv.first)); break;
					case ShaderDataType::Sampler2D:
					case ShaderDataType::SamplerCube:
					{
						Asset<Texture> texture;
						BS_ASSERT(stream.serialize(texture));
						material->SetTexture(kv.first, 0, texture);
						break;
					}
					default: break;
					}
				}
			}

			return true;
		}
	};
}