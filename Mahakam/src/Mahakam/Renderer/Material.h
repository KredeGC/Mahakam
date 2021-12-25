#pragma once

#include "Buffer.h"
#include "Shader.h"
#include "Texture.h"

#include <glm/glm.hpp>

namespace Mahakam
{
	class Material
	{
	public:
		virtual ~Material() = default;

		virtual Ref<Shader> getShader() const = 0;

		virtual void bind() const = 0;

		virtual void setTransform(const glm::mat4& modelMatrix) = 0;

		virtual void setTransformIndex(int index, const glm::mat4& modelMatrix) = 0;

		virtual void setTexture(const std::string& name, int slot, Ref<Texture> tex) = 0;

		virtual void setMat3(const std::string& name, const glm::mat3& value) = 0;
		virtual void setMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual void setInt(const std::string& name, int value) = 0;
		
		virtual void setFloat(const std::string& name, float value) = 0;
		virtual void setFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void setFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void setFloat4(const std::string& name, const glm::vec4& value) = 0;


		virtual const Ref<Texture> getTexture(const std::string& name) const = 0;
					 
		virtual const glm::mat3 getMat3(const std::string& name) const = 0;
		virtual const glm::mat4 getMat4(const std::string& name) const = 0;
					 
		virtual int getInt(const std::string& name) const = 0;
		inline int getSampler(const std::string& name) const { return getInt(name); }
					 
		virtual float getFloat(const std::string& name) const = 0;
		virtual const glm::vec2 getFloat2(const std::string& name) const = 0;
		virtual const glm::vec3 getFloat3(const std::string& name) const = 0;
		virtual const glm::vec4 getFloat4(const std::string& name) const = 0;

		static Ref<Material> create(Ref<Shader> shader);
	};
}