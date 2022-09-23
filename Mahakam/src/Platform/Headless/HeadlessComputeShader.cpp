#include "Mahakam/mhpch.h"
#include "HeadlessComputeShader.h"

#include "Mahakam/Core/FileUtility.h"
#include "Mahakam/Core/Profiler.h"

#include "Mahakam/Renderer/Texture.h"

#include <filesystem>
#include <fstream>

namespace Mahakam
{
	HeadlessComputeShader::HeadlessComputeShader(const std::filesystem::path& filepath)
		: m_Filepath(filepath)
	{
		MH_PROFILE_FUNCTION();

		std::string fileString = filepath.string();

		// Naming
		auto lastSlash = fileString.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;

		auto lastDot = fileString.rfind(".");
		auto count = lastDot == std::string::npos ? fileString.size() - lastSlash : lastDot - lastSlash;

		m_Name = fileString.substr(lastSlash, count);
	}

	HeadlessComputeShader::~HeadlessComputeShader() {}

	void HeadlessComputeShader::Bind() const {}

	void HeadlessComputeShader::Dispatch(uint32_t x, uint32_t y, uint32_t z) {}

	void HeadlessComputeShader::SetTexture(const std::string& name, Asset<Texture> tex) {}
}