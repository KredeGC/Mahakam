#pragma once

#include "Mahakam/Version.h"

#include <memory>
#ifndef MH_DEBUG
#include <robin_hood/robin_hood.h>
#else
#include <unordered_map>
#include <unordered_set>
#endif


#ifdef MH_DEBUG
#define MH_ENABLE_PROFILING // Enable profiling in debug builds
#define MH_ENABLE_RENDER_PROFILING // Enable render profiling in debug builds
#define MH_ENABLE_ASSERT // Enable asserts in debug builds
#define MH_ENABLE_GL_ERRORS // Enable render API errors in debug builds
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
#define MH_BREAKPOINT()

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

// Function declarations which should be reachable in DLLs
#ifndef MH_STANDALONE
#define MH_DECLARE_FUNC_LINE2(line, func, returnType, ...) static returnType (*func)(__VA_ARGS__); \
	inline static uint8_t generated_##line = (::Mahakam::SharedLibrary::AddFunction((void**)&func), 0);
#define MH_DECLARE_FUNC_LINE(line, func, returnType, ...) MH_DECLARE_FUNC_LINE2(line, func, returnType, __VA_ARGS__)

#define MH_DECLARE_FUNC(func, returnType, ...) MH_DECLARE_FUNC_LINE(__LINE__, func, returnType, __VA_ARGS__)
#define MH_DEFINE_FUNC(func, returnType, ...) returnType (*func)(__VA_ARGS__) = [](__VA_ARGS__) -> returnType
#else
#define MH_DECLARE_FUNC(func, returnType, ...) static returnType func(__VA_ARGS__);
#define MH_DEFINE_FUNC(func, returnType, ...) returnType func(__VA_ARGS__)
#endif

namespace Mahakam
{
#ifdef MH_DEBUG
	template<typename K, typename V>
	using UnorderedMap = std::unordered_map<K, V>;

	template<typename K>
	using UnorderedSet = std::unordered_set<K>;
#else
	template<typename K, typename V>
	using UnorderedMap = robin_hood::unordered_map<K, V>;

	template<typename K>
	using UnorderedSet = robin_hood::unordered_set<K>;
#endif

	typedef void** FuncPtr;

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T>
	constexpr Ref<T> DynamicCastRef(Ref<void> ptr)
	{
		return std::dynamic_pointer_cast<T>(ptr);
	}

	template<typename T>
	constexpr Ref<T> StaticCastRef(Ref<void> ptr)
	{
		return std::static_pointer_cast<T>(ptr);
	}

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