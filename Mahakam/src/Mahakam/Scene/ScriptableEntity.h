#pragma once

#include "Entity.h"
#include "Mahakam/Core/Timestep.h"

namespace Mahakam
{
	class ScriptableEntity
	{
	private:
		friend class Scene;

	protected:
		virtual void onCreate() {}
		virtual void onDestroy() {}
		virtual void onUpdate(Timestep ts) {}

	public:
		virtual ~ScriptableEntity() = default;

		Entity entity;

		template<typename T>
		T& getComponent() const { return entity.getComponent<T>(); }
	};
}