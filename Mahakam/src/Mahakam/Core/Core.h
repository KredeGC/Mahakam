#pragma once

#ifndef MH_STANDALONE
#define MH_ENABLE_DYNAMIC_LINKING // Expose engine functions to dynamically loaded libraries
#define MH_ENABLE_PROFILING // Enable profiling in builds
#define MH_ENABLE_RENDER_PROFILING // Enable render profiling in builds
#ifdef MH_DEBUG
#define MH_ENABLE_ASSERT // Enable asserts in debug builds
#define MH_ENABLE_GL_ERRORS // Enable render API error logging in debug builds
#endif // MH_DEBUG
#endif // MH_STANDALONE

#include "Mahakam/Version.h"


#ifdef MH_ENABLE_ASSERT
#if defined(MH_PLATFORM_WINDOWS)
#define MH_BREAKPOINT() __debugbreak()
#elif defined(MH_PLATFORM_LINUX)
#include <csignal>
#define MH_BREAKPOINT() std::raise(SIGTRAP)
#else
#define MH_BREAKPOINT()
#endif

#define MH_BREAK(...) { MH_ERROR("Assertion failed: {0}", __VA_ARGS__); MH_BREAKPOINT(); }

#define MH_ASSERT(x, ...) { if(!(x)) { MH_BREAK(__VA_ARGS__) } }
#else // MH_ENABLE_ASSERT
#define MH_BREAKPOINT()

#define MH_BREAK(...)

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

// Returns a non-capturing lambda which calls fn
#define MH_BIND_EVENT(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }