#include "Mahakam/mhpch.h"
#include "HeadlessTexture.h"

#include "Mahakam/Core/Log.h"
#include "Mahakam/Core/Profiler.h"

namespace Mahakam
{
	HeadlessTexture2D::HeadlessTexture2D(const TextureProps& props)
		: m_Props(props)
	{
		MH_PROFILE_FUNCTION();

		// Calculate the size
		m_Size = 0;
		m_TotalSize = 0;
	}

	HeadlessTexture2D::HeadlessTexture2D(const std::filesystem::path& filepath, const TextureProps& props)
		: m_Filepath(filepath), m_Props(props)
	{
		MH_PROFILE_FUNCTION();

		m_Props.Width = 0;
		m_Props.Height = 0;

		// Calculate the size
		m_Size = 0;
		m_TotalSize = 0;
	}

	void HeadlessTexture2D::SetData(void* data, uint32_t size, bool mipmaps)
	{
		
	}

	void HeadlessTexture2D::Bind(uint32_t slot) const
	{
		
	}

	void HeadlessTexture2D::BindImage(uint32_t slot, bool read, bool write) const
	{
		
	}

	void HeadlessTexture2D::ReadPixels(void* pixels, bool mipmaps)
	{
		
	}

	void HeadlessTexture2D::Resize(uint32_t width, uint32_t height)
	{
		MH_PROFILE_FUNCTION();

		m_Props.Width = width;
		m_Props.Height = height;
	}



	HeadlessTextureCube::HeadlessTextureCube(const CubeTextureProps& props)
		: m_Props(props)
	{
		MH_PROFILE_FUNCTION();

		// Calculate the size
		m_Size = 0;
		m_TotalSize = 0;
	}

	HeadlessTextureCube::HeadlessTextureCube(const std::filesystem::path& filepath, const CubeTextureProps& props)
		: m_Filepath(filepath), m_Props(props)
	{
		MH_PROFILE_FUNCTION();

		// Calculate the size
		m_Size = 0;
		m_TotalSize = 0;
	}

	void HeadlessTextureCube::Resize(uint32_t width, uint32_t height)
	{
		MH_BREAK("UNSUPPORTED");
	}

	void HeadlessTextureCube::SetData(void* data, uint32_t size, bool mipmaps)
	{
		
	}

	void HeadlessTextureCube::Bind(uint32_t slot) const
	{
		
	}

	void HeadlessTextureCube::BindImage(uint32_t slot, bool read, bool write) const
	{
		
	}

	void HeadlessTextureCube::ReadPixels(void* pixels, bool mipmaps)
	{
		
	}
}