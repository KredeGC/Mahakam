#include "Mahakam/mhpch.h"

#include "MeshSerialization.h"

#include "AssetSerialization.h"
#include "FilepathSerialization.h"

namespace c4::yml
{
	template<typename V>
	static void Serialize(ryml::NodeRef& node, const c4::csubstr& name, const V& value)
	{
		node[name] << value;
	}

	template<typename V>
	static bool Deserialize(const ryml::NodeRef& node, const c4::csubstr& name, V& value)
	{
		if (!node.has_child(name))
			return false;

		node[name] >> value;

		return true;
	}

	// MeshProps
	void write(ryml::NodeRef* n, Mahakam::MeshProps const& val)
	{
		*n |= ryml::MAP;

		ryml::NodeRef materialsNode = (*n)["Materials"];
		materialsNode |= ryml::SEQ;

		for (auto& material : val.Materials)
			materialsNode.append_child() << material;
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

		return true;
	}

	// BoneMeshProps
	void write(ryml::NodeRef* n, Mahakam::BoneMeshProps const& val)
	{
		*n << val.Base;

		Serialize(*n, "Filepath", val.Filepath);
		Serialize(*n, "IncludeNodes", val.IncludeNodes);
		Serialize(*n, "IncludeBones", val.IncludeBones);
	}

	bool read(ryml::NodeRef const& n, Mahakam::BoneMeshProps* val)
	{
		n >> val->Base;

		Deserialize(n, "Filepath", val->Filepath);
		Deserialize(n, "IncludeNodes", val->IncludeNodes);
		Deserialize(n, "IncludeBones", val->IncludeBones);

		return true;
	}

	// CubeMeshProps
	void write(ryml::NodeRef* n, Mahakam::CubeMeshProps const& val)
	{
		*n << val.Base;

		Serialize(*n, "Tessellation", val.Tessellation);
		Serialize(*n, "Invert", val.Invert);
	}

	bool read(ryml::NodeRef const& n, Mahakam::CubeMeshProps* val)
	{
		n >> val->Base;

		Deserialize(n, "Tessellation", val->Tessellation);
		Deserialize(n, "Invert", val->Invert);

		return true;
	}

	// CubeSphereMeshProps
	void write(ryml::NodeRef* n, Mahakam::CubeSphereMeshProps const& val)
	{
		*n << val.Base;

		Serialize(*n, "Tessellation", val.Tessellation);
		Serialize(*n, "Invert", val.Invert);
	}

	bool read(ryml::NodeRef const& n, Mahakam::CubeSphereMeshProps* val)
	{
		n >> val->Base;

		Deserialize(n, "Tessellation", val->Tessellation);
		Deserialize(n, "Invert", val->Invert);

		return true;
	}

	// PlaneMeshProps
	void write(ryml::NodeRef* n, Mahakam::PlaneMeshProps const& val)
	{
		*n << val.Base;

		Serialize(*n, "Rows", val.Rows);
		Serialize(*n, "Columns", val.Columns);
	}

	bool read(ryml::NodeRef const& n, Mahakam::PlaneMeshProps* val)
	{
		n >> val->Base;

		Deserialize(n, "Rows", val->Rows);
		Deserialize(n, "Columns", val->Columns);

		return true;
	}

	// UVSphereMeshProps
	void write(ryml::NodeRef* n, Mahakam::UVSphereMeshProps const& val)
	{
		*n << val.Base;

		Serialize(*n, "Rows", val.Rows);
		Serialize(*n, "Columns", val.Columns);
	}

	bool read(ryml::NodeRef const& n, Mahakam::UVSphereMeshProps* val)
	{
		n >> val->Base;

		Deserialize(n, "Rows", val->Rows);
		Deserialize(n, "Columns", val->Columns);

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