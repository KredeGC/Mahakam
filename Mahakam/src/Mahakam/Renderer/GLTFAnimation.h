#pragma once

namespace std::filesystem
{
	class path;
}

namespace Mahakam
{
	class GLTFAnimation
	{
	public:
		GLTFAnimation(const std::filesystem::path& filepath);

		inline static Asset<GLTFAnimation> Load(const std::filesystem::path& filepath) { return LoadImpl(filepath); }

	private:
		static Asset<GLTFAnimation> LoadImpl(const std::filesystem::path& filepath);
	};
}