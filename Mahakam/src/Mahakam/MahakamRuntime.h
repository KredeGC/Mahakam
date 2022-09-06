#pragma once

#include <Mahakam/Mahakam.h>

#ifdef MH_STANDALONE

#define MH_EXPORTED
#define MH_EXTERN_EXPORTED
#define MH_RUNTIME_LOAD(context, funcPtrs)

#else // MH_STANDALONE

#if defined _WIN32 || defined __CYGWIN__
#ifdef MH_WIN_EXPORT
#ifdef __GNUC__
#define MH_EXPORTED __attribute__ ((dllexport))
#else
#define MH_EXPORTED __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#endif
#else
#ifdef __GNUC__
#define MH_EXPORTED __attribute__ ((dllimport))
#else
#define MH_EXPORTED __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#else
#if __GNUC__ >= 4
#define MH_EXPORTED __attribute__ ((visibility ("default")))
#else
#define MH_EXPORTED
#endif
#endif

#define MH_EXTERN_EXPORTED extern "C" MH_EXPORTED

#define MH_RUNTIME_LOAD(context, funcPtrs) ImGui::SetCurrentContext(context); \
	SharedLibrary::ImportFuncPointers(funcPtrs);

#endif // MH_STANDALONE