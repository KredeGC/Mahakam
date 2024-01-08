#pragma once

#include "Mahakam/Renderer/Texture.h"

#include "AssetSerialization.h"
#include "MathSerialization.h"

#include <bitstream.h>

namespace bitstream
{
	template<>
	struct serialize_traits<Mahakam::TextureCube>
	{
		template<typename Stream>
		typename utility::is_writing_t<Stream>
		static serialize(Stream& writer, const Mahakam::Asset<Mahakam::TextureCube>& texture)
		{
			/*writer.serialize<TextureFormat>(textureAsset->GetProps().Format);
			writer.serialize<TextureFilter>(textureAsset->GetProps().FilterMode);
			writer.serialize<uint32_t>(textureAsset->GetProps().Resolution);
			writer.serialize<TextureCubePrefilter>(textureAsset->GetProps().Prefilter);
			writer.serialize<bool>(textureAsset->GetProps().Mipmaps);*/

			void* pixels;

			texture->ReadPixels(pixels);

			return true;
		}

		template<typename Stream>
		typename utility::is_reading_t<Stream>
		static Deserialize(Stream& reader, Mahakam::Asset<Mahakam::TextureCube>& texture)
		{
			Mahakam::CubeTextureProps props;

			/*reader.serialize<TextureFormat>(props.Format);
			reader.serialize<TextureFilter>(props.FilterMode);
			reader.serialize<uint32_t>(props.Resolution);
			reader.serialize<TextureCubePrefilter>(props.Prefilter);
			reader.serialize<bool>(props.Mipmaps);*/

			return true;
		}
	};
}