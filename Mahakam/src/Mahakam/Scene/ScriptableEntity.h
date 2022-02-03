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
		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(Timestep ts) {}

	public:
		virtual ~ScriptableEntity() = default;

		Entity entity;

		template<typename T>
		T& GetComponent() const { return entity.GetComponent<T>(); }
	};
}