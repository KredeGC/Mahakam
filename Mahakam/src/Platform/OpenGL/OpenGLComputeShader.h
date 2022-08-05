#pragma once

#include "Mahakam/Renderer/ComputeShader.h"

#include <robin_hood.h>

namespace Mahakam
{
	class OpenGLComputeShader : public ComputeShader
	{
	private:
		uint32_t m_RendererID;
		std::filesystem::path m_Filepath;
		std::string m_Name;

		UnorderedMap<std::string, int> m_UniformIDCache;

	public:
		OpenGLComputeShader(const std::filesystem::path& filepath);
		virtual ~OpenGLComputeShader();

		virtual void Bind() const;

		virtual void Dispatch(uint32_t x, uint32_t y, uint32_t z);

		virtual void SetTexture(const std::string& name, Asset<Texture> tex);

	private:
		void CompileBinary(const std::filesystem::path& cachePath, const std::string& src);

		int GetUniformLocation(const std::string& name);
	};
}