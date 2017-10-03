// Copyright (c) 2012-2013 Bartosz Szczesny
// LICENSE: The MIT License (MIT)

#ifndef PIXMAP_PNG_H // enables .png format, requires '-lpng' compilation flag
#define PIXMAP_PNG_H

#include "pixmap.h"
#include <png.h>

int pixmap_write_png(const struct pixmap* img, const char* filename);

#endif
