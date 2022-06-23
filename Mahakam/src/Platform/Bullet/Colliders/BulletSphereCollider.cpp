#include "mhpch.h"
#include "BulletSphereCollider.h"

namespace Mahakam
{
	BulletSphereCollider::BulletSphereCollider(float radius)
		: m_Shape(radius)
	{
		m_Radius = radius;
	}

	BulletSphereCollider::~BulletSphereCollider()
	{

	}

	void BulletSphereCollider::SetRadius(float value)
	{
		m_Shape = btSphereShape(value);
		m_Radius = value;
	}
}