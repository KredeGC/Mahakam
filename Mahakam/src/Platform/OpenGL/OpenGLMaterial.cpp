#include "mhpch.h"
#include "OpenGLMaterial.h"

#include "Mahakam/Renderer/GL.h"
#include "Mahakam/Renderer/Material.h"
#include "Mahakam/Renderer/Texture.h"

namespace Mahakam
{
	OpenGLMaterial::OpenGLMaterial(const Asset<Shader>& shader, const std::string& variant)
		: m_Shader(shader), m_Variant(variant)
	{
		MH_PROFILE_FUNCTION();

		auto& defaultProps = shader->GetProperties();

		ResetShaderProperties(defaultProps);
	}

	OpenGLMaterial::OpenGLMaterial(const Asset<Material>& material) :
		m_Shader(static_cast<Asset<OpenGLMaterial>>(material)->m_Shader),
		m_Variant(static_cast<Asset<OpenGLMaterial>>(material)->m_Variant),
		m_Textures(static_cast<Asset<OpenGLMaterial>>(material)->m_Textures),
		m_Mat3s(static_cast<Asset<OpenGLMaterial>>(material)->m_Mat3s),
		m_Mat4s(static_cast<Asset<OpenGLMaterial>>(material)->m_Mat4s),
		m_Ints(static_cast<Asset<OpenGLMaterial>>(material)->m_Ints),
		m_Floats(static_cast<Asset<OpenGLMaterial>>(material)->m_Floats),
		m_Float2s(static_cast<Asset<OpenGLMaterial>>(material)->m_Float2s),
		m_Float3s(static_cast<Asset<OpenGLMaterial>>(material)->m_Float3s),
		m_Float4s(static_cast<Asset<OpenGLMaterial>>(material)->m_Float4s) {}

	uint64_t OpenGLMaterial::Hash() const
	{
		uint64_t hash = 2166136261ULL;

		for (auto& kv : m_Textures)
		{
			unsigned char bytes[sizeof(Texture*)];
			memcpy(bytes, &kv.second, sizeof(Texture*));

			for (uint64_t i = 0; i < sizeof(Texture*); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : m_Mat3s)
		{
			unsigned char bytes[sizeof(glm::mat3)];
			memcpy(bytes, &kv.second, sizeof(glm::mat3));

			for (uint64_t i = 0; i < sizeof(glm::mat3); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : m_Mat4s)
		{
			unsigned char bytes[sizeof(glm::mat4)];
			memcpy(bytes, &kv.second, sizeof(glm::mat4));

			for (uint64_t i = 0; i < sizeof(glm::mat4); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : m_Ints)
		{
			unsigned char bytes[sizeof(int32_t)];
			memcpy(bytes, &kv.second, sizeof(int32_t));

			for (uint64_t i = 0; i < sizeof(int32_t); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : m_Floats)
		{
			unsigned char bytes[sizeof(float)];
			memcpy(bytes, &kv.second, sizeof(float));

			for (uint64_t i = 0; i < sizeof(float); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : m_Float2s)
		{
			unsigned char bytes[sizeof(glm::vec2)];
			memcpy(bytes, &kv.second, sizeof(glm::vec2));

			for (uint64_t i = 0; i < sizeof(glm::vec2); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : m_Float3s)
		{
			unsigned char bytes[sizeof(glm::vec3)];
			memcpy(bytes, &kv.second, sizeof(glm::vec3));

			for (uint64_t i = 0; i < sizeof(glm::vec3); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : m_Float4s)
		{
			unsigned char bytes[sizeof(glm::vec4)];
			memcpy(bytes, &kv.second, sizeof(glm::vec4));

			for (uint64_t i = 0; i < sizeof(glm::vec4); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		return hash;
	}

	void OpenGLMaterial::Bind()
	{
		//shader->bind();

		for (auto& kv : m_Textures)
			m_Shader->SetTexture(kv.first, kv.second);

		for (auto& kv : m_Mat3s)
			m_Shader->SetUniformMat3(kv.first, kv.second);

		for (auto& kv : m_Mat4s)
			m_Shader->SetUniformMat4(kv.first, kv.second);

		for (auto& kv : m_Ints)
			m_Shader->SetUniformInt(kv.first, kv.second);

		for (auto& kv : m_Floats)
			m_Shader->SetUniformFloat(kv.first, kv.second);

		for (auto& kv : m_Float2s)
			m_Shader->SetUniformFloat2(kv.first, kv.second);

		for (auto& kv : m_Float3s)
			m_Shader->SetUniformFloat3(kv.first, kv.second);

		for (auto& kv : m_Float4s)
			m_Shader->SetUniformFloat4(kv.first, kv.second);
	}

	void OpenGLMaterial::SetTransform(const glm::mat4& modelMatrix)
	{
		m_Shader->SetUniformMat4("u_m4_M", modelMatrix);
	}

	Asset<Texture> OpenGLMaterial::GetTexture(const std::string& name) const
	{
		auto texIter = m_Textures.find(name);
		if (texIter != m_Textures.end())
			return texIter->second;

		MH_CORE_WARN("Material ({0}) has no texture named {1}", GetShader()->GetName(), name);

		return nullptr;
	}

	glm::mat3 OpenGLMaterial::GetMat3(const std::string& name) const
	{
		auto matIter = m_Mat3s.find(name);
		if (matIter != m_Mat3s.end())
			return matIter->second;

		MH_CORE_WARN("Material ({0}) has no mat3 named {1}", GetShader()->GetName(), name);

		return glm::mat3(1.0f);
	}

	glm::mat4 OpenGLMaterial::GetMat4(const std::string& name) const
	{
		auto matIter = m_Mat4s.find(name);
		if (matIter != m_Mat4s.end())
			return matIter->second;

		MH_CORE_WARN("Material ({0}) has no mat4 named {1}", GetShader()->GetName(), name);

		return glm::mat4(1.0f);
	}

	int32_t OpenGLMaterial::GetInt(const std::string& name) const
	{
		auto intIter = m_Ints.find(name);
		if (intIter != m_Ints.end())
			return intIter->second;

		MH_CORE_WARN("Material ({0}) has no int named {1}", GetShader()->GetName(), name);

		return 0;
	}

	float OpenGLMaterial::GetFloat(const std::string& name) const
	{
		auto floatIter = m_Floats.find(name);
		if (floatIter != m_Floats.end())
			return floatIter->second;

		MH_CORE_WARN("Material ({0}) has no float named {1}", GetShader()->GetName(), name);

		return 0.0f;
	}

	glm::vec2 OpenGLMaterial::GetFloat2(const std::string& name) const
	{
		auto floatIter = m_Float2s.find(name);
		if (floatIter != m_Float2s.end())
			return floatIter->second;

		MH_CORE_WARN("Material ({0}) has no float2 named {1}", GetShader()->GetName(), name);

		return glm::vec2(0.0f);
	}

	glm::vec3 OpenGLMaterial::GetFloat3(const std::string& name) const
	{
		auto floatIter = m_Float3s.find(name);
		if (floatIter != m_Float3s.end())
			return floatIter->second;

		MH_CORE_WARN("Material ({0}) has no float3 named {1}", GetShader()->GetName(), name);

		return glm::vec3(0.0f);
	}

	glm::vec4 OpenGLMaterial::GetFloat4(const std::string& name) const
	{
		auto floatIter = m_Float4s.find(name);
		if (floatIter != m_Float4s.end())
			return floatIter->second;

		MH_CORE_WARN("Material ({0}) has no float4 named {1}", GetShader()->GetName(), name);

		return glm::vec4(0.0f);
	}

	void OpenGLMaterial::ResetShaderProperties(const UnorderedMap<std::string, ShaderProperty>& properties)
	{
		for (auto& prop : properties)
		{
			switch (prop.second.DataType)
			{
			case ShaderDataType::Sampler2D:
				m_Textures[prop.first] = GL::GetTexture2DWhite();
				break;
			case ShaderDataType::SamplerCube:
				m_Textures[prop.first] = GL::GetTextureCubeWhite();
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
				MH_CORE_WARN("Material properties for {0} shader include unused property: {1}", m_Shader->GetName(), prop.first);
				break;
			}
		}
	}
}