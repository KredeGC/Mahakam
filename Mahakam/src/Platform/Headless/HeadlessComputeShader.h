#pragma once

#include "Mahakam/Renderer/ComputeShader.h"

namespace Mahakam
{
	class HeadlessComputeShader : public ComputeShader
	{
	private:
		std::filesystem::path m_Filepath;
		std::string m_Name;

	public:
		HeadlessComputeShader(const std::filesystem::path& filepath);
		virtual ~HeadlessComputeShader();

		virtual void Bind() const;

		virtual void Dispatch(uint32_t x, uint32_t y, uint32_t z);

		virtual void SetTexture(const std::string& name, Asset<Texture> tex);
	};
}