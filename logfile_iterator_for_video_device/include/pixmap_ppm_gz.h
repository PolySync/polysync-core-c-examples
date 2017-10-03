// Copyright (c) 2012-2014 Bartosz Szczesny <bszcz@bszcz.org>
// This program is free software under the MIT license.

#ifndef PIXMAP_PPM_GZ_H // enables .ppm.gz format, requires '-lz' compilation flag
#define PIXMAP_PPM_GZ_H

#include "pixmap.h"
#include <zlib.h>

int pixmap_write_ppm_gz(const struct pixmap* img, const char* filename);

#endif
