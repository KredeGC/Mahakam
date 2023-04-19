#pragma once

#include "SceneDef.h"
#include "Entity.h"
#include "Components/CameraComponent.h"

namespace Mahakam
{
	template<typename Fn>
	void Scene::ForEachEntity(Fn func)
	{
		TrivialVector<entt::entity> entities;
		m_Registry.each([&](entt::entity entity)
		{
			entities.push_back(entity);
		});

		for (auto iter = entities.rbegin(); iter != entities.rend(); ++iter)
			func(Entity{ *iter, this });
	}

	template<typename Fn>
	void Scene::ForEachEntityReverse(Fn func)
	{
		return m_Registry.each(func);
	}

	template<>
	inline void Scene::OnComponentAdded<CameraComponent>(const Entity& entity, CameraComponent& component)
	{
		component.GetCamera().SetRatio(m_ViewportRatio);
	}
}