#pragma once

#include "Mahakam/Asset/Asset.h"

#include <ryml/rapidyaml-0.4.1.hpp>

namespace c4::yml
{
	template<typename T>
	void write(ryml::NodeRef* n, Mahakam::Asset<T> val)
	{
		*n << val.GetID();
	}

	template<typename T>
	bool read(ryml::NodeRef const& n, Mahakam::Asset<T>* val)
	{
		Mahakam::AssetDatabase::AssetID id;
		n >> id;
		*val = Mahakam::Asset<T>(id);
		return true;
	}
}