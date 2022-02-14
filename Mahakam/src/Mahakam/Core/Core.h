#pragma once

#include <memory>


#ifdef MH_DEBUG
#define MH_ENABLE_ASSERT // Enable asserts in debug builds
#define MH_ENABLE_GL_ERRORS // Enable render API errors in debug builds
#define MH_ENABLE_RENDER_PROFILING // Enable render profiling in debug builds
#endif // MH_DEBUG

//#define MH_ENABLE_RENDER_PROFILING // Enable render profiling in all builds
#define MH_ENABLE_PROFILING // Enable profiling in all builds


#ifdef MH_ENABLE_ASSERT
#if defined(MH_PLATFORM_WINDOWS)
#define MH_BREAKPOINT() __debugbreak()
#elif defined(MH_PLATFORM_LINUX)
#define MH_BREAKPOINT() raise(SIGTRAP)
#else
#define MH_BREAKPOINT()
#endif

#define MH_CORE_BREAK(...) { MH_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); MH_BREAKPOINT(); }
#define MH_BREAK(...) { MH_ERROR("Assertion failed: {0}", __VA_ARGS__); MH_BREAKPOINT(); }

#define MH_CORE_ASSERT(x, ...) { if(!(x)) { MH_CORE_BREAK(__VA_ARGS__) } }
#define MH_ASSERT(x, ...) { if(!(x)) { MH_BREAK(__VA_ARGS__) } }
#else // MH_ENABLE_ASSERT
#define MH_CORE_BREAK(...)
#define MH_BREAK(...)

#define MH_CORE_ASSERT(x, ...)
#define MH_ASSERT(x, ...)
#endif // MH_ENABLE_ASSERT

#ifdef MH_ENABLE_PROFILING
#define MH_PROFILE_SCOPE_LINE2(name, line, flushRenderer) ::Mahakam::Profiler profiler##line(name, flushRenderer);
#define MH_PROFILE_SCOPE_LINE(name, line, flushRenderer) MH_PROFILE_SCOPE_LINE2(name, line, flushRenderer)

#define MH_PROFILE_SCOPE(name) MH_PROFILE_SCOPE_LINE(name, __LINE__, false)
#define MH_PROFILE_FUNCTION() MH_PROFILE_SCOPE(__FUNCTION__)

#define MH_PROFILE_RENDERING_SCOPE(name) MH_PROFILE_SCOPE_LINE(name, __LINE__, true)
#define MH_PROFILE_RENDERING_FUNCTION() MH_PROFILE_RENDERING_SCOPE(__FUNCTION__)
#else // MH_ENABLE_PROFILING
#define MH_PROFILE_SCOPE(name)
#define MH_PROFILE_FUNCTION()

#define MH_PROFILE_RENDERING_SCOPE(name)
#define MH_PROFILE_RENDERING_FUNCTION()
#endif // MH_ENABLE_PROFILING

#define BIT(x) (1 << x)

#define MH_BIND_EVENT(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Mahakam
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#include "Log.h"
#include "Profiler.h"