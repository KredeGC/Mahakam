#include "mhpch.h"
#include "OpenGLMaterial.h"

namespace Mahakam
{
	OpenGLMaterial::OpenGLMaterial(const Ref<Shader>& shader, const ShaderProps& props)
		: shader(std::static_pointer_cast<OpenGLShader>(shader))
	{
		auto& defaultProps = shader->getProperties();

		for (auto& prop : defaultProps)
		{
			switch (prop.dataType)
			{
			case ShaderDataType::Sampler2D:
				textures[prop.location] = Texture2D::white;
				break;
			case ShaderDataType::SamplerCube:
				textures[prop.location] = TextureCube::white;
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

		// TODO: Initialize using the specified props
	}

	OpenGLMaterial::OpenGLMaterial(const Ref<Material>& material)
		: shader(std::static_pointer_cast<OpenGLMaterial>(material)->shader),
		textures(std::static_pointer_cast<OpenGLMaterial>(material)->textures),
		mat3s(std::static_pointer_cast<OpenGLMaterial>(material)->mat3s),
		mat4s(std::static_pointer_cast<OpenGLMaterial>(material)->mat4s),
		ints(std::static_pointer_cast<OpenGLMaterial>(material)->ints),
		floats(std::static_pointer_cast<OpenGLMaterial>(material)->floats),
		float2s(std::static_pointer_cast<OpenGLMaterial>(material)->float2s),
		float3s(std::static_pointer_cast<OpenGLMaterial>(material)->float3s),
		float4s(std::static_pointer_cast<OpenGLMaterial>(material)->float4s) {}

	void OpenGLMaterial::bind() const
	{
		//shader->bind();

		for (auto& kv : ints)
			shader->setUniformInt(kv.first, kv.second);

		for (auto& kv : textures)
			kv.second->bind(kv.first);

		for (auto& kv : mat3s)
			shader->setUniformMat3(kv.first, kv.second);

		for (auto& kv : mat4s)
			shader->setUniformMat4(kv.first, kv.second);

		for (auto& kv : floats)
			shader->setUniformFloat(kv.first, kv.second);

		for (auto& kv : float2s)
			shader->setUniformFloat2(kv.first, kv.second);

		for (auto& kv : float3s)
			shader->setUniformFloat3(kv.first, kv.second);

		for (auto& kv : float4s)
			shader->setUniformFloat4(kv.first, kv.second);
	}

	void OpenGLMaterial::setTransform(const glm::mat4& modelMatrix)
	{
		shader->setUniformMat4("u_m4_M", modelMatrix);
	}

	Ref<Texture> OpenGLMaterial::getTexture(const std::string& name) const
	{
		auto& intIter = ints.find(name);
		if (intIter != ints.end())
		{
			auto& texIter = textures.find(intIter->second);
			if (texIter != textures.end())
				return texIter->second;
		}

		MH_CORE_WARN("Material ({0}) has no texture named {0}", getShader()->getName(), name);

		return nullptr;
	}

	const glm::mat3 OpenGLMaterial::getMat3(const std::string& name) const
	{
		auto& matIter = mat3s.find(name);
		if (matIter != mat3s.end())
			return matIter->second;

		MH_CORE_WARN("Material ({0}) has no mat3 named {0}", getShader()->getName(), name);

		return glm::mat3(1.0f);
	}

	const glm::mat4 OpenGLMaterial::getMat4(const std::string& name) const
	{
		auto& matIter = mat4s.find(name);
		if (matIter != mat4s.end())
			return matIter->second;

		MH_CORE_WARN("Material ({0}) has no mat4 named {0}", getShader()->getName(), name);

		return glm::mat4(1.0f);
	}

	int OpenGLMaterial::getInt(const std::string& name) const
	{
		auto& intIter = ints.find(name);
		if (intIter != ints.end())
			return intIter->second;

		MH_CORE_WARN("Material ({0}) has no int named {0}", getShader()->getName(), name);

		return 0;
	}

	float OpenGLMaterial::getFloat(const std::string& name) const
	{
		auto& floatIter = floats.find(name);
		if (floatIter != floats.end())
			return floatIter->second;

		MH_CORE_WARN("Material ({0}) has no float named {0}", getShader()->getName(), name);

		return 0.0f;
	}

	const glm::vec2 OpenGLMaterial::getFloat2(const std::string& name) const
	{
		auto& floatIter = float2s.find(name);
		if (floatIter != float2s.end())
			return floatIter->second;

		MH_CORE_WARN("Material ({0}) has no float2 named {0}", getShader()->getName(), name);

		return glm::vec2(0.0f);
	}

	const glm::vec3 OpenGLMaterial::getFloat3(const std::string& name) const
	{
		auto& floatIter = float3s.find(name);
		if (floatIter != float3s.end())
			return floatIter->second;

		MH_CORE_WARN("Material ({0}) has no float3 named {0}", getShader()->getName(), name);

		return glm::vec3(0.0f);
	}

	const glm::vec4 OpenGLMaterial::getFloat4(const std::string& name) const
	{
		auto& floatIter = float4s.find(name);
		if (floatIter != float4s.end())
			return floatIter->second;

		MH_CORE_WARN("Material ({0}) has no float4 named {0}", getShader()->getName(), name);

		return glm::vec4(0.0f);
	}
}