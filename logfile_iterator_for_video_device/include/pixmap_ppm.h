// Copyright (c) 2012-2014 Bartosz Szczesny <bszcz@bszcz.org>
// This program is free software under the MIT license.

#ifndef PIXMAP_PPM_H // enables .ppm format
#define PIXMAP_PPM_H

#include "pixmap.h"

int pixmap_write_ppm(const struct pixmap* img, const char* filename);

#endif
