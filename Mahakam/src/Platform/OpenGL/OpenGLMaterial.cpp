#include "Mahakam/mhpch.h"
#include "OpenGLMaterial.h"

#include "OpenGLShader.h"

#include "Mahakam/Core/Allocator.h"
#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Profiler.h"

#include "Mahakam/Renderer/Buffer.h"
#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/Texture.h"

namespace Mahakam
{
	template<typename T>
	static void CopyUniformToData(const UnorderedMap<std::string, ShaderProperty>& properties, const std::string& name, uint8_t* data, const void* value)
	{
		std::string key = name;
		int offset = 0;

		size_t startPar = name.find("[");
		size_t endPar = name.find("]");
		if (startPar != std::string::npos && endPar != std::string::npos)
		{
			key = name.substr(0, startPar);
			offset = std::stoi(name.substr(startPar + 1, endPar - startPar - 1)) * sizeof(T);
		}

		auto iter = properties.find(key);
		if (iter != properties.end())
		{
			if (iter->second.Offset != -1)
				std::memcpy(data + iter->second.Offset + offset, value, sizeof(T));
		}
		else
		{
			MH_WARN("Attempting to set nonexistent property ({0})", key);
		}
	}

	OpenGLMaterial::OpenGLMaterial(Asset<Shader> shader)
		: m_Shader(std::move(shader))
	{
		MH_PROFILE_FUNCTION();

		auto& defaultProps = m_Shader->GetProperties();

		ResetShaderProperties(defaultProps);

		m_DataSize = m_Shader->GetUniformSize();
		if (m_DataSize > 0)
			m_Data = Allocator::Allocate<uint8_t>(m_DataSize);
	}

	OpenGLMaterial::OpenGLMaterial(const OpenGLMaterial& other) :
		m_Shader(other.m_Shader),
		m_Data(nullptr),
		m_DataSize(other.m_DataSize),
		m_Textures(other.m_Textures),
		m_Mat3s(other.m_Mat3s),
		m_Mat4s(other.m_Mat4s),
		m_Ints(other.m_Ints),
		m_Floats(other.m_Floats),
		m_Float2s(other.m_Float2s),
		m_Float3s(other.m_Float3s),
		m_Float4s(other.m_Float4s)
	{
		if (m_DataSize > 0)
		{
			m_Data = Allocator::Allocate<uint8_t>(m_DataSize);
			std::memcpy(m_Data, other.m_Data, other.m_DataSize);
		}
	}

	OpenGLMaterial::OpenGLMaterial(OpenGLMaterial&& other) noexcept :
		m_Shader(std::move(other.m_Shader)),
		m_Data(other.m_Data),
		m_DataSize(other.m_DataSize),
		m_Textures(std::move(other.m_Textures)),
		m_Mat3s(std::move(other.m_Mat3s)),
		m_Mat4s(std::move(other.m_Mat4s)),
		m_Ints(std::move(other.m_Ints)),
		m_Floats(std::move(other.m_Floats)),
		m_Float2s(std::move(other.m_Float2s)),
		m_Float3s(std::move(other.m_Float3s)),
		m_Float4s(std::move(other.m_Float4s))
	{
		other.m_Data = nullptr;
	}

	OpenGLMaterial::~OpenGLMaterial()
	{
		if (m_Data)
			Allocator::Deallocate<uint8_t>(m_Data, m_DataSize);
	}

	OpenGLMaterial& OpenGLMaterial::operator=(const OpenGLMaterial& rhs)
	{
		m_Shader = rhs.m_Shader;
		m_Textures = rhs.m_Textures;
		m_Mat3s = rhs.m_Mat3s;
		m_Mat4s = rhs.m_Mat4s;
		m_Ints = rhs.m_Ints;
		m_Floats = rhs.m_Floats;
		m_Float2s = rhs.m_Float2s;
		m_Float3s = rhs.m_Float3s;
		m_Float4s = rhs.m_Float4s;

		m_DataSize = rhs.m_DataSize;
		if (m_DataSize > 0)
		{
			m_Data = Allocator::Allocate<uint8_t>(m_DataSize);
			std::memcpy(m_Data, rhs.m_Data, rhs.m_DataSize);
		}
		else
		{
			m_Data = nullptr;
		}

		return *this;
	}

	OpenGLMaterial& OpenGLMaterial::operator=(OpenGLMaterial&& rhs) noexcept
	{
		m_Shader = std::move(rhs.m_Shader);
		m_Textures = std::move(rhs.m_Textures);
		m_Mat3s = std::move(rhs.m_Mat3s);
		m_Mat4s = std::move(rhs.m_Mat4s);
		m_Ints = std::move(rhs.m_Ints);
		m_Floats = std::move(rhs.m_Floats);
		m_Float2s = std::move(rhs.m_Float2s);
		m_Float3s = std::move(rhs.m_Float3s);
		m_Float4s = std::move(rhs.m_Float4s);

		m_DataSize = rhs.m_DataSize;
		m_Data = rhs.m_Data;

		rhs.m_Data = nullptr;

		return *this;
	}

	uint64_t OpenGLMaterial::Hash() const
	{
		uint64_t hash = 2166136261ULL;

		for (auto& kv : m_Textures)
		{
			unsigned char bytes[sizeof(Texture*)];
			std::memcpy(bytes, &kv.second, sizeof(Texture*));

			for (uint64_t i = 0; i < sizeof(Texture*); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : m_Mat3s)
		{
			unsigned char bytes[sizeof(glm::mat3)];
			std::memcpy(bytes, &kv.second, sizeof(glm::mat3));

			for (uint64_t i = 0; i < sizeof(glm::mat3); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : m_Mat4s)
		{
			unsigned char bytes[sizeof(glm::mat4)];
			std::memcpy(bytes, &kv.second, sizeof(glm::mat4));

			for (uint64_t i = 0; i < sizeof(glm::mat4); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : m_Ints)
		{
			unsigned char bytes[sizeof(int32_t)];
			std::memcpy(bytes, &kv.second, sizeof(int32_t));

			for (uint64_t i = 0; i < sizeof(int32_t); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : m_Floats)
		{
			unsigned char bytes[sizeof(float)];
			std::memcpy(bytes, &kv.second, sizeof(float));

			for (uint64_t i = 0; i < sizeof(float); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : m_Float2s)
		{
			unsigned char bytes[sizeof(glm::vec2)];
			std::memcpy(bytes, &kv.second, sizeof(glm::vec2));

			for (uint64_t i = 0; i < sizeof(glm::vec2); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : m_Float3s)
		{
			unsigned char bytes[sizeof(glm::vec3)];
			std::memcpy(bytes, &kv.second, sizeof(glm::vec3));

			for (uint64_t i = 0; i < sizeof(glm::vec3); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : m_Float4s)
		{
			unsigned char bytes[sizeof(glm::vec4)];
			std::memcpy(bytes, &kv.second, sizeof(glm::vec4));

			for (uint64_t i = 0; i < sizeof(glm::vec4); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		return hash;
	}

	void OpenGLMaterial::BindShader(const std::string& shaderPass)
	{
		m_Shader->Bind(shaderPass);
	}

	void OpenGLMaterial::Bind(UniformBuffer& uniformBuffer)
	{
		for (auto& [name, texture] : m_Textures)
			m_Shader->SetTexture(name, *texture);

		if (m_Data && m_DataSize > 0)
			uniformBuffer.SetData(m_Data, 0, m_DataSize);
	}

	void OpenGLMaterial::SetMat3(const std::string& name, const glm::mat3& value)
	{
		m_Mat3s[name] = value;

		CopyUniformToData<glm::mat3>(m_Shader->GetProperties(), name, m_Data, glm::value_ptr(value));
	}

	void OpenGLMaterial::SetMat4(const std::string& name, const glm::mat4& value)
	{
		m_Mat4s[name] = value;

		CopyUniformToData<glm::mat4>(m_Shader->GetProperties(), name, m_Data, glm::value_ptr(value));
	}

	void OpenGLMaterial::SetInt(const std::string& name, int32_t value)
	{
		m_Ints[name] = value;

		CopyUniformToData<int>(m_Shader->GetProperties(), name, m_Data, &value);
	}

	void OpenGLMaterial::SetFloat(const std::string& name, float value)
	{
		m_Floats[name] = value;

		CopyUniformToData<float>(m_Shader->GetProperties(), name, m_Data, &value);
	}

	void OpenGLMaterial::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		m_Float2s[name] = value;

		CopyUniformToData<glm::vec2>(m_Shader->GetProperties(), name, m_Data, glm::value_ptr(value));
	}

	void OpenGLMaterial::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		m_Float3s[name] = value;

		CopyUniformToData<glm::vec3>(m_Shader->GetProperties(), name, m_Data, glm::value_ptr(value));
	}

	void OpenGLMaterial::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		m_Float4s[name] = value;

		CopyUniformToData<glm::vec4>(m_Shader->GetProperties(), name, m_Data, glm::value_ptr(value));
	}

	Asset<Texture> OpenGLMaterial::GetTexture(const std::string& name) const
	{
		auto texIter = m_Textures.find(name);
		if (texIter != m_Textures.end())
			return texIter->second;

		MH_WARN("Material ({0}) has no texture named {1}", GetShader()->GetName(), name);

		return nullptr;
	}

	glm::mat3 OpenGLMaterial::GetMat3(const std::string& name) const
	{
		auto matIter = m_Mat3s.find(name);
		if (matIter != m_Mat3s.end())
			return matIter->second;

		MH_WARN("Material ({0}) has no mat3 named {1}", GetShader()->GetName(), name);

		return glm::mat3(1.0f);
	}

	glm::mat4 OpenGLMaterial::GetMat4(const std::string& name) const
	{
		auto matIter = m_Mat4s.find(name);
		if (matIter != m_Mat4s.end())
			return matIter->second;

		MH_WARN("Material ({0}) has no mat4 named {1}", GetShader()->GetName(), name);

		return glm::mat4(1.0f);
	}

	int32_t OpenGLMaterial::GetInt(const std::string& name) const
	{
		auto intIter = m_Ints.find(name);
		if (intIter != m_Ints.end())
			return intIter->second;

		MH_WARN("Material ({0}) has no int named {1}", GetShader()->GetName(), name);

		return 0;
	}

	float OpenGLMaterial::GetFloat(const std::string& name) const
	{
		auto floatIter = m_Floats.find(name);
		if (floatIter != m_Floats.end())
			return floatIter->second;

		MH_WARN("Material ({0}) has no float named {1}", GetShader()->GetName(), name);

		return 0.0f;
	}

	glm::vec2 OpenGLMaterial::GetFloat2(const std::string& name) const
	{
		auto floatIter = m_Float2s.find(name);
		if (floatIter != m_Float2s.end())
			return floatIter->second;

		MH_WARN("Material ({0}) has no float2 named {1}", GetShader()->GetName(), name);

		return glm::vec2(0.0f);
	}

	glm::vec3 OpenGLMaterial::GetFloat3(const std::string& name) const
	{
		auto floatIter = m_Float3s.find(name);
		if (floatIter != m_Float3s.end())
			return floatIter->second;

		MH_WARN("Material ({0}) has no float3 named {1}", GetShader()->GetName(), name);

		return glm::vec3(0.0f);
	}

	glm::vec4 OpenGLMaterial::GetFloat4(const std::string& name) const
	{
		auto floatIter = m_Float4s.find(name);
		if (floatIter != m_Float4s.end())
			return floatIter->second;

		MH_WARN("Material ({0}) has no float4 named {1}", GetShader()->GetName(), name);

		return glm::vec4(0.0f);
	}

	void OpenGLMaterial::ResetShaderProperties(const UnorderedMap<std::string, ShaderProperty>& properties)
	{
		for (auto& prop : properties)
		{
			switch (prop.second.DataType)
			{
			case ShaderDataType::Sampler2D:
				m_Textures[prop.first] = Asset<Texture2D>(GL::GetTexture2DWhite());
				break;
			case ShaderDataType::SamplerCube:
				m_Textures[prop.first] = Asset<TextureCube>(GL::GetTextureCubeWhite());
				break;
			case ShaderDataType::Float:
				m_Floats[prop.first] = 0.0f;
				break;
			case ShaderDataType::Float2:
				m_Float2s[prop.first] = glm::vec2(0.0f);
				break;
			case ShaderDataType::Float3:
				m_Float3s[prop.first] = glm::vec3(0.0f);
				break;
			case ShaderDataType::Float4:
				m_Float4s[prop.first] = glm::vec4(0.0f);
				break;
			case ShaderDataType::Mat3:
				m_Mat3s[prop.first] = glm::mat3(1.0f);
				break;
			case ShaderDataType::Mat4:
				m_Mat4s[prop.first] = glm::mat4(1.0f);
				break;
			case ShaderDataType::Int:
				m_Ints[prop.first] = 0;
				break;
			default:
				MH_WARN("Material properties for {0} shader include unused property: {1}", m_Shader->GetName(), prop.first);
				break;
			}
		}
	}
}