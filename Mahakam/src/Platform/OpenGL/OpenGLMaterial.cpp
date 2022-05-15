#include "mhpch.h"
#include "OpenGLMaterial.h"

#include "Mahakam/Renderer/GL.h"

namespace Mahakam
{
	OpenGLMaterial::OpenGLMaterial(const Asset<Shader>& shader, const std::string& variant)
		: shader(shader), variant(variant)
	{
		MH_PROFILE_FUNCTION();

		auto& defaultProps = shader->GetProperties();

		ResetShaderProperties(defaultProps);
	}

	OpenGLMaterial::OpenGLMaterial(const Asset<Material>& material) :
		shader(static_cast<Asset<OpenGLMaterial>>(material)->shader),
		variant(static_cast<Asset<OpenGLMaterial>>(material)->variant),
		textures(static_cast<Asset<OpenGLMaterial>>(material)->textures),
		mat3s(static_cast<Asset<OpenGLMaterial>>(material)->mat3s),
		mat4s(static_cast<Asset<OpenGLMaterial>>(material)->mat4s),
		ints(static_cast<Asset<OpenGLMaterial>>(material)->ints),
		floats(static_cast<Asset<OpenGLMaterial>>(material)->floats),
		float2s(static_cast<Asset<OpenGLMaterial>>(material)->float2s),
		float3s(static_cast<Asset<OpenGLMaterial>>(material)->float3s),
		float4s(static_cast<Asset<OpenGLMaterial>>(material)->float4s) {}

	uint64_t OpenGLMaterial::Hash() const
	{
		uint64_t hash = 2166136261ULL;

		for (auto& kv : textures)
		{
			unsigned char bytes[sizeof(Texture*)];
			memcpy(bytes, &kv.second, sizeof(Texture*));

			for (uint64_t i = 0; i < sizeof(Texture*); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : mat3s)
		{
			unsigned char bytes[sizeof(glm::mat3)];
			memcpy(bytes, &kv.second, sizeof(glm::mat3));

			for (uint64_t i = 0; i < sizeof(glm::mat3); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : mat4s)
		{
			unsigned char bytes[sizeof(glm::mat4)];
			memcpy(bytes, &kv.second, sizeof(glm::mat4));

			for (uint64_t i = 0; i < sizeof(glm::mat4); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : ints)
		{
			unsigned char bytes[sizeof(int32_t)];
			memcpy(bytes, &kv.second, sizeof(int32_t));

			for (uint64_t i = 0; i < sizeof(int32_t); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : floats)
		{
			unsigned char bytes[sizeof(float)];
			memcpy(bytes, &kv.second, sizeof(float));

			for (uint64_t i = 0; i < sizeof(float); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : float2s)
		{
			unsigned char bytes[sizeof(glm::vec2)];
			memcpy(bytes, &kv.second, sizeof(glm::vec2));

			for (uint64_t i = 0; i < sizeof(glm::vec2); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : float3s)
		{
			unsigned char bytes[sizeof(glm::vec3)];
			memcpy(bytes, &kv.second, sizeof(glm::vec3));

			for (uint64_t i = 0; i < sizeof(glm::vec3); ++i)
				hash = (hash * 16777619ULL) ^ bytes[i];
		}

		for (auto& kv : float4s)
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

		for (auto& kv : textures)
			shader->SetTexture(kv.first, kv.second);

		for (auto& kv : mat3s)
			shader->SetUniformMat3(kv.first, kv.second);

		for (auto& kv : mat4s)
			shader->SetUniformMat4(kv.first, kv.second);

		for (auto& kv : ints)
			shader->SetUniformInt(kv.first, kv.second);

		for (auto& kv : floats)
			shader->SetUniformFloat(kv.first, kv.second);

		for (auto& kv : float2s)
			shader->SetUniformFloat2(kv.first, kv.second);

		for (auto& kv : float3s)
			shader->SetUniformFloat3(kv.first, kv.second);

		for (auto& kv : float4s)
			shader->SetUniformFloat4(kv.first, kv.second);
	}

	void OpenGLMaterial::SetTransform(const glm::mat4& modelMatrix)
	{
		shader->SetUniformMat4("u_m4_M", modelMatrix);
	}

	Asset<Texture> OpenGLMaterial::GetTexture(const std::string& name) const
	{
		auto texIter = textures.find(name);
		if (texIter != textures.end())
			return texIter->second;

		MH_CORE_WARN("Material ({0}) has no texture named {1}", GetShader()->GetName(), name);

		return nullptr;
	}

	const glm::mat3 OpenGLMaterial::GetMat3(const std::string& name) const
	{
		auto matIter = mat3s.find(name);
		if (matIter != mat3s.end())
			return matIter->second;

		MH_CORE_WARN("Material ({0}) has no mat3 named {1}", GetShader()->GetName(), name);

		return glm::mat3(1.0f);
	}

	const glm::mat4 OpenGLMaterial::GetMat4(const std::string& name) const
	{
		auto matIter = mat4s.find(name);
		if (matIter != mat4s.end())
			return matIter->second;

		MH_CORE_WARN("Material ({0}) has no mat4 named {1}", GetShader()->GetName(), name);

		return glm::mat4(1.0f);
	}

	int32_t OpenGLMaterial::GetInt(const std::string& name) const
	{
		auto intIter = ints.find(name);
		if (intIter != ints.end())
			return intIter->second;

		MH_CORE_WARN("Material ({0}) has no int named {1}", GetShader()->GetName(), name);

		return 0;
	}

	float OpenGLMaterial::GetFloat(const std::string& name) const
	{
		auto floatIter = floats.find(name);
		if (floatIter != floats.end())
			return floatIter->second;

		MH_CORE_WARN("Material ({0}) has no float named {1}", GetShader()->GetName(), name);

		return 0.0f;
	}

	const glm::vec2 OpenGLMaterial::GetFloat2(const std::string& name) const
	{
		auto floatIter = float2s.find(name);
		if (floatIter != float2s.end())
			return floatIter->second;

		MH_CORE_WARN("Material ({0}) has no float2 named {1}", GetShader()->GetName(), name);

		return glm::vec2(0.0f);
	}

	const glm::vec3 OpenGLMaterial::GetFloat3(const std::string& name) const
	{
		auto floatIter = float3s.find(name);
		if (floatIter != float3s.end())
			return floatIter->second;

		MH_CORE_WARN("Material ({0}) has no float3 named {1}", GetShader()->GetName(), name);

		return glm::vec3(0.0f);
	}

	const glm::vec4 OpenGLMaterial::GetFloat4(const std::string& name) const
	{
		auto floatIter = float4s.find(name);
		if (floatIter != float4s.end())
			return floatIter->second;

		MH_CORE_WARN("Material ({0}) has no float4 named {1}", GetShader()->GetName(), name);

		return glm::vec4(0.0f);
	}

	void OpenGLMaterial::ResetShaderProperties(const std::unordered_map<std::string, ShaderProperty>& properties)
	{
		for (auto& prop : properties)
		{
			switch (prop.second.DataType)
			{
			case ShaderDataType::Sampler2D:
				textures[prop.first] = GL::GetTexture2DWhite();
				break;
			case ShaderDataType::SamplerCube:
				textures[prop.first] = GL::GetTextureCubeWhite();
				break;
			case ShaderDataType::Float:
				floats[prop.first] = 0.0f;
				break;
			case ShaderDataType::Float2:
				float2s[prop.first] = glm::vec2(0.0f);
				break;
			case ShaderDataType::Float3:
				float3s[prop.first] = glm::vec3(0.0f);
				break;
			case ShaderDataType::Float4:
				float4s[prop.first] = glm::vec4(0.0f);
				break;
			case ShaderDataType::Mat3:
				mat3s[prop.first] = glm::mat3(1.0f);
				break;
			case ShaderDataType::Mat4:
				mat4s[prop.first] = glm::mat4(1.0f);
				break;
			case ShaderDataType::Int:
				ints[prop.first] = 0;
				break;
			default:
				MH_CORE_WARN("Material properties for {0} shader include unused property: {1}", shader->GetName(), prop.first);
				break;
			}
		}
	}
}