#pragma once

#include "asf.h"

typedef struct
{
	const uint8_t* bitmaps;
	const uint8_t width_pixels;
	const uint8_t width_bytes;
	const uint8_t height_pixels;
} SPRITE_INFO;

extern const SPRITE_INFO missed_call;