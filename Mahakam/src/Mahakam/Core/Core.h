#pragma once

#include <memory>

#ifdef MH_DEBUG
#define MH_CORE_BREAK(...) { MH_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); }
#define MH_BREAK(...) { MH_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); }

#define MH_PROFILE_SCOPE_LINE2(name, line, flushRenderer) ::Mahakam::Profiler profiler##line(name, flushRenderer);
#define MH_PROFILE_SCOPE_LINE(name, line, flushRenderer) MH_PROFILE_SCOPE_LINE2(name, line, flushRenderer)

#define MH_PROFILE_SCOPE(name) MH_PROFILE_SCOPE_LINE(name, __LINE__, false)
#define MH_PROFILE_FUNCTION() MH_PROFILE_SCOPE(__FUNCTION__)

#define MH_PROFILE_RENDERING_SCOPE(name) MH_PROFILE_SCOPE_LINE(name, __LINE__, true)
#define MH_PROFILE_RENDERING_FUNCTION() MH_PROFILE_RENDERING_SCOPE(__FUNCTION__)

#define MH_CORE_ASSERT(x, ...) { if(!(x)) { MH_CORE_BREAK(__VA_ARGS__) } }
#define MH_ASSERT(x, ...) { if(!(x)) { MH_BREAK(__VA_ARGS__) } }
#else
#define MH_CORE_BREAK(...)
#define MH_BREAK(...)

#define MH_PROFILE_SCOPE(name)
#define MH_PROFILE_FUNCTION()

#define MH_PROFILE_RENDERING_SCOPE(name)
#define MH_PROFILE_RENDERING_FUNCTION()

#define MH_CORE_ASSERT(x, ...)
#define MH_ASSERT(x, ...)
#endif

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