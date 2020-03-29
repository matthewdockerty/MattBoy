#include "Tile.h"

#include <memory>

Tile::Tile()
{
	memset(pixelsPalette_, sizeof(pixelsPalette_[0][0]) * TILE_SIZE * TILE_SIZE, 0);
}


Tile::~Tile()
{
}

void Tile::SetLSBitsForLine(int tileLine, uint8_t value)
{
	for (int i = 0; i < 8; i++) {
		pixelsPalette_[tileLine][8 - i - 1] = static_cast<uint8_t>(pixelsPalette_[tileLine][8 - i - 1] & 0b10 | (value >> i) & 0b1);
	}
}

void Tile::SetMSBitsForLine(int tileLine, uint8_t value)
{
	for (int i = 0; i < 8; i++) {
		pixelsPalette_[tileLine][8 - i - 1] = static_cast<uint8_t>((value >> (i - 1)) & 0b10 | pixelsPalette_[tileLine][8 - i - 1] & 0b1);
	}
}
