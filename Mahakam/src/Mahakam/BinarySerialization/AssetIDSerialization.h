#pragma once

#include "SerializeTraits.h"
#include "IntegralSerialization.h"

#include "Mahakam/Asset/Asset.h"

namespace Mahakam::Serialization
{
	template<typename T>
	struct SerializeTraits<Asset<T>>
	{
		template<typename Stream>
		typename utility::is_writing_t<Stream>
		static serialize(Stream& writer, const Asset<T>& asset) noexcept
		{
			return writer.serialize(asset.GetID());
		}

		template<typename Stream>
		typename utility::is_reading_t<Stream>
		static serialize(Stream& reader, Asset<T>& asset) noexcept
		{
			AssetID assetID;
			if (!reader.serialize(assetID))
				return false;

			asset = Asset<T>(assetID);

			return true;
		}
	};
}