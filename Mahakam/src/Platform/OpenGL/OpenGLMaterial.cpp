#include "mhpch.h"
#include "OpenGLMaterial.h"

namespace Mahakam
{
	OpenGLMaterial::OpenGLMaterial(Ref<Shader> shader)
		: shader(std::dynamic_pointer_cast<OpenGLShader>(shader)) { }

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

	void OpenGLMaterial::setTransformIndex(int index, const glm::mat4& modelMatrix)
	{
		shader->setUniformMat4("u_m4_M[" + std::to_string(index) + "]", modelMatrix);
	}

	const Ref<Texture> OpenGLMaterial::getTexture(const std::string& name) const
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