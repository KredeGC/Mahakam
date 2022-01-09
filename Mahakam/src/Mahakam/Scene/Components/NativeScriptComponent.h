#pragma once

#include "Mahakam/Scene/ScriptableEntity.h"

namespace Mahakam
{
	struct NativeScriptComponent
	{
	public:
		struct RuntimeScript
		{
			ScriptableEntity* (*instantiateScript)();
			void (*destroyScript)(ScriptableEntity*);
			ScriptableEntity* script = nullptr;
		};

		std::vector<RuntimeScript> scripts;

		template<typename T>
		void bind()
		{
			scripts.emplace_back(RuntimeScript{
				[]() { return static_cast<ScriptableEntity*>(new T()); },
				[](ScriptableEntity* script) { delete (T*)script; }
			});
		}

		void unbind(int index)
		{
			scripts.erase(scripts.begin() + index);
		}
	};
}