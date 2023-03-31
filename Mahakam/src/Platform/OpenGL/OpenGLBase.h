#pragma once
#include "Mahakam/Core/Core.h"
#include "Mahakam/Core/Log.h"

#include <glad/gl.h>

#ifdef MH_ENABLE_GL_ERRORS
namespace Mahakam
{
	inline void glClearErrors()
	{
		for (int i = 0; i < 10 && (glGetError() != GL_NO_ERROR); i++);
	}

	inline bool glLogCall(const char* name, const char* file, int line)
	{
		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR)
		{
			MH_ERROR("[OpenGL Error] (Code {0}): {1} {2}:{3}", error, name, file, line);
			return false;
		}

		return true;
	}
}

#define MH_GL_CALL(x) { ::Mahakam::glClearErrors();\
	x;\
	MH_ASSERT(::Mahakam::glLogCall(#x, __FILE__, __LINE__), "OpenGL Error occurred!"); }
#else
#define MH_GL_CALL(x) x;
#endif