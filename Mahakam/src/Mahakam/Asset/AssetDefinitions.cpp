#include "Mahakam/mhpch.h"

#include "Mahakam/Asset/Asset.h"

namespace Mahakam
{
	// Audio
	class Sound;

	// Renderer
	class Animation;
	class ComputeShader;
	class FrameBuffer;
	class Material;
	class Mesh;
	class SubMesh;
	class RenderBuffer;
	class Shader;
	class Texture;
	class Texture2D;
	class Texture2DArray;
	class Texture3D;
	class TextureCube;

	// Headless
	class HeadlessComputeShader;
	class HeadlessFrameBuffer;
	class HeadlessMaterial;
	class HeadlessMesh;
	class HeadlessRenderBuffer;
	class HeadlessShader;
	class HeadlessTexture2D;
	class HeadlessTextureCube;

	// OpenGL
	class OpenGLComputeShader;
	class OpenGLFrameBuffer;
	class OpenGLMaterial;
	class OpenGLMesh;
	class OpenGLRenderBuffer;
	class OpenGLShader;
	class OpenGLTexture2D;
	class OpenGLTextureCube;

	// SteamAudio
	class MiniAudioSound;
}

// Audio
template class Mahakam::Asset<Mahakam::Sound>;

// Renderer
template class Mahakam::Asset<Mahakam::Animation>;
template class Mahakam::Asset<Mahakam::ComputeShader>;
template class Mahakam::Asset<Mahakam::FrameBuffer>;
template class Mahakam::Asset<Mahakam::Material>;
template class Mahakam::Asset<Mahakam::Mesh>;
template class Mahakam::Asset<Mahakam::SubMesh>;
template class Mahakam::Asset<Mahakam::RenderBuffer>;
template class Mahakam::Asset<Mahakam::Shader>;
template class Mahakam::Asset<Mahakam::Texture>;
template class Mahakam::Asset<Mahakam::Texture2D>;
template class Mahakam::Asset<Mahakam::Texture2DArray>;
template class Mahakam::Asset<Mahakam::Texture3D>;
template class Mahakam::Asset<Mahakam::TextureCube>;

// Headless
template class Mahakam::Asset<Mahakam::HeadlessComputeShader>;
template class Mahakam::Asset<Mahakam::HeadlessFrameBuffer>;
template class Mahakam::Asset<Mahakam::HeadlessMaterial>;
template class Mahakam::Asset<Mahakam::HeadlessMesh>;
template class Mahakam::Asset<Mahakam::HeadlessRenderBuffer>;
template class Mahakam::Asset<Mahakam::HeadlessShader>;
template class Mahakam::Asset<Mahakam::HeadlessTexture2D>;
template class Mahakam::Asset<Mahakam::HeadlessTextureCube>;

// OpenGL
template class Mahakam::Asset<Mahakam::OpenGLComputeShader>;
template class Mahakam::Asset<Mahakam::OpenGLFrameBuffer>;
template class Mahakam::Asset<Mahakam::OpenGLMaterial>;
template class Mahakam::Asset<Mahakam::OpenGLMesh>;
template class Mahakam::Asset<Mahakam::OpenGLRenderBuffer>;
template class Mahakam::Asset<Mahakam::OpenGLShader>;
template class Mahakam::Asset<Mahakam::OpenGLTexture2D>;
template class Mahakam::Asset<Mahakam::OpenGLTextureCube>;

// SteamAudio
template class Mahakam::Asset<Mahakam::MiniAudioSound>;