// Copyright (c) 2012-2014 Bartosz Szczesny <bszcz@bszcz.org>
// This program is free software under the MIT license.

#ifndef PIXMAP_H
#define PIXMAP_H

#include <stdio.h>
#include <stdlib.h>

// chroma sampling
enum {
	PIXMAP_CHROMA_411 = 1,	// 4:1:1
	PIXMAP_CHROMA_422 = 2,	// 4:2:2
	PIXMAP_CHROMA_444 = 4	// 4:4:4
};

// colour channels
enum {
	PIXMAP_RED = 0,
	PIXMAP_GREEN = 1,
	PIXMAP_BLUE = 2,
	PIXMAP_COLORS = 3
};

// good values for jpg compression
enum {
	PIXMAP_GOOD_CHROMA = PIXMAP_CHROMA_444,
	PIXMAP_GOOD_QUALITY = 80
};

struct pixmap {
	int width;
	int height;
	int chroma;  // jpeg only
	int quality; // jpeg only
	unsigned char*   bytes;   // access by: bytes[0..PIXMAP_COLORS*width*height] (RGBRGB... in row major order)
	unsigned char**  pixels;  // access by: pixels[0..width*height][PIXMAP_RED..PIXMAP_BLUE]
	unsigned char*** pixels2; // access by: pixels2[0..width][0..height][PIXMAP_RED..PIXMAP_BLUE]
};

void* pixmap_free(struct pixmap* img);

void* pixmap_alloc_error(struct pixmap* img);

struct pixmap* pixmap_alloc(int width, int height);

#endif
