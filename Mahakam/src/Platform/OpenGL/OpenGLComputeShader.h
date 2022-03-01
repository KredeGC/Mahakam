#pragma once

#include "Mahakam/Renderer/ComputeShader.h"

#include <robin_hood.h>

namespace Mahakam
{
	class OpenGLComputeShader : public ComputeShader
	{
	private:
		uint32_t rendererID;
		std::string filepath;
		std::string name;

		robin_hood::unordered_map<std::string, int> uniformIDCache;

	public:
		OpenGLComputeShader(const std::string& filepath);
		virtual ~OpenGLComputeShader();

		virtual void Bind() const;

		virtual void Dispatch(uint32_t x, uint32_t y, uint32_t z);

		virtual void SetTexture(const std::string & name, Ref<Texture> tex);

	private:
		void CompileBinary(const std::string& cachePath, const std::string& src);

		int GetUniformLocation(const std::string& name);
	};
}