#pragma once

#include "Mahakam/Core/Log.h"

#include <ryml/rapidyaml-0.4.1.hpp>

namespace Mahakam
{
	class YAMLGuard
	{
	private:
		struct ErrorCallback
		{
			// However, it is important to note that the error callback must never
			// return to the caller! Otherwise, an infinite loop or program crash
			// may occur.

			// this will be called on error
			void on_error(const char* msg, size_t len, ryml::Location loc)
			{
#ifdef __cpp_exceptions
				throw std::runtime_error(ryml::formatrs<std::string>("{}:{}:{} ({}B): ERROR: {}",
					loc.name, loc.line, loc.col, loc.offset, ryml::csubstr(msg, len)));
#else
				// Infinite loops you say :)?
				MH_ERROR("[rapidyaml Error] {0}", msg);
#endif
			}

			// bridge
			ryml::Callbacks callbacks()
			{
				return ryml::Callbacks(this, nullptr, nullptr, ErrorCallback::s_error);
			}
			static void s_error(const char* msg, size_t len, ryml::Location loc, void* this_)
			{
				(static_cast<ErrorCallback*>(this_))->on_error(msg, len, loc);
			}
		};

		static ErrorCallback s_Callback;

	public:
		static void Init();

		static void Shutdown();
	};
}