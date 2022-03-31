#pragma once

#include <memory>


#ifdef MH_DEBUG
#define MH_ENABLE_PROFILING // Enable profiling in debug builds
#define MH_ENABLE_ASSERT // Enable asserts in debug builds
#define MH_ENABLE_GL_ERRORS // Enable render API errors in debug builds
#define MH_ENABLE_RENDER_PROFILING // Enable render profiling in debug builds
#endif // MH_DEBUG

//#define MH_ENABLE_RENDER_PROFILING // Enable render profiling in all builds
//#define MH_ENABLE_PROFILING // Enable profiling in all builds


#ifdef MH_ENABLE_ASSERT
#if defined(MH_PLATFORM_WINDOWS)
#define MH_BREAKPOINT() __debugbreak()
#elif defined(MH_PLATFORM_LINUX)
#include <csignal>
#define MH_BREAKPOINT() std::raise(SIGTRAP)
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
#define MH_PROFILE_SCOPE_LINE2(name, line, flushRenderer) ::Mahakam::Profiler profiler##line = Profiler::Create(name, flushRenderer);
#define MH_PROFILE_SCOPE_LINE(name, line, flushRenderer) MH_PROFILE_SCOPE_LINE2(name, line, flushRenderer)

#define MH_PROFILE_SCOPE(name) MH_PROFILE_SCOPE_LINE(name, __LINE__, false)
#define MH_PROFILE_FUNCTION() MH_PROFILE_SCOPE(__FUNCTION__)

#define MH_PROFILE_RENDERING_SCOPE(name) MH_PROFILE_SCOPE_LINE(name, __LINE__, true)
#define MH_PROFILE_RENDERING_FUNCTION() MH_PROFILE_RENDERING_SCOPE(__FUNCTION__)

#define MH_PROFILE_BEGIN_SESSION(name, filepath) ::Mahakam::Instrumentor::Get().BeginSession(name, filepath)
#define MH_PROFILE_END_SESSION() ::Mahakam::Instrumentor::Get().EndSession()
#else // MH_ENABLE_PROFILING
#define MH_PROFILE_SCOPE(name)
#define MH_PROFILE_FUNCTION()

#define MH_PROFILE_RENDERING_SCOPE(name)
#define MH_PROFILE_RENDERING_FUNCTION()

#define MH_PROFILE_BEGIN_SESSION(name, filepath)
#define MH_PROFILE_END_SESSION()
#endif // MH_ENABLE_PROFILING

// Returns the bitness of x
#define BIT(x) (1 << x)

// Returns a non-capturing lambda which calls fn
#define MH_BIND_EVENT(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

// Defines a typdef function pointer and initializes it as a static variable
#define MH_SHARED_FUNC(returnType, type, ...) typedef returnType(*type)(__VA_ARGS__); \
	inline static type sh_##type = nullptr;

// Calls the SharedLibrary function and returns the result
#define MH_OVERRIDE_FUNC(signature, ...) if (SharedLibrary::sh_##signature != nullptr) \
	return SharedLibrary::sh_##signature(__VA_ARGS__);

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

	template<typename T>
	using WeakRef = std::weak_ptr<T>;

	template<typename T>
	constexpr WeakRef<T> CreateWeakRef(const Ref<T>& ref)
	{
		return WeakRef<T>(ref);
	}
}

#include "Log.h"
#include "Profiler.h"
#include "Instrumentor.h"