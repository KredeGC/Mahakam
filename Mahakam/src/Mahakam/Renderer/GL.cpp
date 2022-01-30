#include "mhpch.h"
#include "GL.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Mahakam
{
	RendererAPI* GL::rendererAPI = new OpenGLRendererAPI();

	void GL::init()
	{
		rendererAPI->init();

		uint8_t whiteData = 255;

		Texture2D::white = Texture2D::create({ 1, 1, TextureFormat::R8, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		Texture2D::white->setData(&whiteData);

		uint8_t blackData = 0;

		Texture2D::black = Texture2D::create({ 1, 1, TextureFormat::R8, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		Texture2D::black->setData(&blackData);

		uint8_t bumpData[3]
		{
			127,
			127,
			255
		};

		Texture2D::bump = Texture2D::create({ 1, 1, TextureFormat::RGB8, TextureFilter::Point, TextureWrapMode::Repeat, TextureWrapMode::Repeat, false });
		Texture2D::bump->setData(&bumpData);
	}
}