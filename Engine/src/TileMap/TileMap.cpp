#include "TileMap/TileMap.hpp"
#include "TileMap/Tile.hpp"

namespace SD {

void TileMap::SetTileMap(const Bitmap &bitmap) {
    m_map =
        Texture::Create(bitmap.Width(), bitmap.Height(),
                        TextureSpec(1, TextureType::TEX_2D, DataFormat::RGBA,
                                    DataFormatType::UBYTE, TextureWrap::EDGE));
    m_map->SetPixels(0, 0, 0, bitmap.Width(), bitmap.Height(), 1,
                     bitmap.Data());
}

}  // namespace SD
