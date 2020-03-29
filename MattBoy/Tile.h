#ifndef TILE_H
#define TILE_H

#include <stdint.h>

class Tile
{
public:
	Tile();
	~Tile();

	static const int TILE_SIZE = 8;

	uint8_t pixelsPalette_[8][8];

	void SetLSBitsForLine(int tileLine, uint8_t value);
	void SetMSBitsForLine(int tileLine, uint8_t value);
};

#endif

