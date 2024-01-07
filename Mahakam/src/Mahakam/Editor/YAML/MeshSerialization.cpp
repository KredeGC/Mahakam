#include "Mahakam/mhpch.h"

#include "MeshSerialization.h"

#include "AssetSerialization.h"

namespace c4::yml
{
	// MeshProps
	void write(ryml::NodeRef* n, Mahakam::MeshProps const& val)
	{
		*n |= ryml::MAP;

		ryml::NodeRef materialsNode = (*n)["Materials"];
		materialsNode |= ryml::SEQ;

		for (auto& material : val.Materials)
			materialsNode.append_child() << material;

		(*n)["IncludeBones"] << val.IncludeBones;
		(*n)["IncludeNodes"] << val.IncludeNodes;
	}

	bool read(ryml::NodeRef const& n, Mahakam::MeshProps* val)
	{
		val->Materials.clear();

		if (n.has_child("Materials"))
		{
			for (auto materialNode : n["Materials"])
			{
				Mahakam::Asset<Mahakam::Material> material;
				materialNode >> material;

				val->Materials.push_back(std::move(material));
			}
		}

		if (n.has_child("IncludeBones"))
			n["IncludeBones"] >> val->IncludeBones;

		if (n.has_child("IncludeNodes"))
			n["IncludeNodes"] >> val->IncludeNodes;

		return true;
	}

	// MeshNode
	void write(ryml::NodeRef* n, Mahakam::MeshNode const& val)
	{
		
	}

	bool read(ryml::NodeRef const& n, Mahakam::MeshNode* val)
	{
		return false;
	}
}