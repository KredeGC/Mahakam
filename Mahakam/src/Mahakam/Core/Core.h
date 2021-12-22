#pragma once

#include <memory>

#ifdef MH_DEBUG
#define MH_CORE_ASSERT(x, ...) { if(!(x)) { MH_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define MH_ASSERT(x, ...) { if(!(x)) { MH_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define MH_CORE_ASSERT(x, ...)
#define MH_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define MH_BIND_EVENT(x) std::bind(&x, this, std::placeholders::_1)

namespace Mahakam
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;
}