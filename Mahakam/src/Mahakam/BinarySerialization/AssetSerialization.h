#pragma once

#include "Mahakam/Asset/Asset.h"

#include <bitstream.h>

namespace bitstream
{
	template<typename T>
	struct serialize_traits<Mahakam::Asset<T>>
	{
		template<typename Stream>
		typename utility::is_writing_t<Stream>
			static serialize(Stream& writer, const Mahakam::Asset<T>& asset) noexcept
		{
			return writer.template serialize<Mahakam::AssetDatabase::AssetID>(asset.GetID());
		}

		template<typename Stream>
		typename utility::is_reading_t<Stream>
			static serialize(Stream& reader, Mahakam::Asset<T>& asset) noexcept
		{
			Mahakam::AssetDatabase::AssetID assetID;
			BS_ASSERT(reader.template serialize<Mahakam::AssetDatabase::AssetID>(assetID));

			asset = Mahakam::Asset<T>(assetID);

			return true;
		}
	};
}