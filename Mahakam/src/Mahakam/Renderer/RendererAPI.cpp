#include "Mahakam/mhpch.h"
#include "RendererAPI.h"

#include "Mahakam/Core/Log.h"

#include "Platform/Headless/HeadlessRendererAPI.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Mahakam
{
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

	//Scope<RendererAPI> RendererAPI::Create()
	MH_DEFINE_FUNC(RendererAPI::Create, Scope<RendererAPI>)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
			return CreateScope<HeadlessRendererAPI>();
		case RendererAPI::API::OpenGL:
			return CreateScope<OpenGLRendererAPI>();
		}

		MH_BREAK("Unknown renderer API!");

		return nullptr;
	};
}