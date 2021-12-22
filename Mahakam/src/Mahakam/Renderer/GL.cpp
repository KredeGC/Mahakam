#include "mhpch.h"
#include "GL.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Mahakam
{
	RendererAPI* GL::rendererAPI = new OpenGLRendererAPI();
}