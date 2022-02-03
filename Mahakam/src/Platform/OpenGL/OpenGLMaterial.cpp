#include "mhpch.h"
#include "OpenGLMaterial.h"

namespace Mahakam
{
	OpenGLMaterial::OpenGLMaterial(const Ref<Shader>& shader, const std::string& variant)
		: shader(std::static_pointer_cast<OpenGLShader>(shader)), variant(variant)
	{
		auto& defaultProps = shader->GetProperties();

		for (auto& prop : defaultProps)
		{
			switch (prop.dataType)
			{
			case ShaderDataType::Sampler2D:
				textures[prop.name] = Texture2D::white;
				break;
			case ShaderDataType::SamplerCube:
				textures[prop.name] = TextureCube::white;
				break;
			case ShaderDataType::Float:
				floats[prop.name] = 0.0f;
				break;
			case ShaderDataType::Float2:
				float2s[prop.name] = glm::vec2(0.0f);
				break;
			case ShaderDataType::Float3:
				float3s[prop.name] = glm::vec3(0.0f);
				break;
			case ShaderDataType::Float4:
				float4s[prop.name] = glm::vec4(0.0f);
				break;
			case ShaderDataType::Mat3:
				mat3s[prop.name] = glm::mat3(1.0f);
				break;
			case ShaderDataType::Mat4:
				mat4s[prop.name] = glm::mat4(1.0f);
				break;
			case ShaderDataType::Int:
				ints[prop.name] = 0;
				break;
			default:
				MH_CORE_BREAK("Unsupported ShaderDataType!");
			}
		}
	}

	OpenGLMaterial::OpenGLMaterial(const Ref<Material>& material) :
		shader(std::static_pointer_cast<OpenGLMaterial>(material)->shader),
		variant(std::static_pointer_cast<OpenGLMaterial>(material)->variant),
		textures(std::static_pointer_cast<OpenGLMaterial>(material)->textures),
		mat3s(std::static_pointer_cast<OpenGLMaterial>(material)->mat3s),
		mat4s(std::static_pointer_cast<OpenGLMaterial>(material)->mat4s),
		ints(std::static_pointer_cast<OpenGLMaterial>(material)->ints),
		floats(std::static_pointer_cast<OpenGLMaterial>(material)->floats),
		float2s(std::static_pointer_cast<OpenGLMaterial>(material)->float2s),
		float3s(std::static_pointer_cast<OpenGLMaterial>(material)->float3s),
		float4s(std::static_pointer_cast<OpenGLMaterial>(material)->float4s) {}

	void OpenGLMaterial::Bind() const
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

	Ref<Texture> OpenGLMaterial::GetTexture(const std::string& name) const
	{
		auto& texIter = textures.find(name);
		if (texIter != textures.end())
			return texIter->second;

		MH_CORE_WARN("Material ({0}) has no texture named {0}", GetShader()->GetName(), name);

		return nullptr;
	}

	const glm::mat3 OpenGLMaterial::GetMat3(const std::string& name) const
	{
		auto& matIter = mat3s.find(name);
		if (matIter != mat3s.end())
			return matIter->second;

		MH_CORE_WARN("Material ({0}) has no mat3 named {0}", GetShader()->GetName(), name);

		return glm::mat3(1.0f);
	}

	const glm::mat4 OpenGLMaterial::GetMat4(const std::string& name) const
	{
		auto& matIter = mat4s.find(name);
		if (matIter != mat4s.end())
			return matIter->second;

		MH_CORE_WARN("Material ({0}) has no mat4 named {0}", GetShader()->GetName(), name);

		return glm::mat4(1.0f);
	}

	int32_t OpenGLMaterial::GetInt(const std::string& name) const
	{
		auto& intIter = ints.find(name);
		if (intIter != ints.end())
			return intIter->second;

		MH_CORE_WARN("Material ({0}) has no int named {0}", GetShader()->GetName(), name);

		return 0;
	}

	float OpenGLMaterial::GetFloat(const std::string& name) const
	{
		auto& floatIter = floats.find(name);
		if (floatIter != floats.end())
			return floatIter->second;

		MH_CORE_WARN("Material ({0}) has no float named {0}", GetShader()->GetName(), name);

		return 0.0f;
	}

	const glm::vec2 OpenGLMaterial::GetFloat2(const std::string& name) const
	{
		auto& floatIter = float2s.find(name);
		if (floatIter != float2s.end())
			return floatIter->second;

		MH_CORE_WARN("Material ({0}) has no float2 named {0}", GetShader()->GetName(), name);

		return glm::vec2(0.0f);
	}

	const glm::vec3 OpenGLMaterial::GetFloat3(const std::string& name) const
	{
		auto& floatIter = float3s.find(name);
		if (floatIter != float3s.end())
			return floatIter->second;

		MH_CORE_WARN("Material ({0}) has no float3 named {0}", GetShader()->GetName(), name);

		return glm::vec3(0.0f);
	}

	const glm::vec4 OpenGLMaterial::GetFloat4(const std::string& name) const
	{
		auto& floatIter = float4s.find(name);
		if (floatIter != float4s.end())
			return floatIter->second;

		MH_CORE_WARN("Material ({0}) has no float4 named {0}", GetShader()->GetName(), name);

		return glm::vec4(0.0f);
	}
}