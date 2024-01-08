#include "Mahakam/mhpch.h"

#include "FilepathSerialization.h"

namespace c4::yml
{
	void write(ryml::NodeRef* n, std::filesystem::path const& val)
	{
		/*char seperator = std::filesystem::path::preferred_separator;

		std::string filepathUnix = val.string();
		std::replace(filepathUnix.begin(), filepathUnix.end(), seperator, '/');*/

		*n << val.generic_string(); // TODO: generic_u8string?
	}

	bool read(ryml::NodeRef const& n, std::filesystem::path* val)
	{
		std::string path;
		n >> path;
		*val = path;
		return true;
	}
}