// Copyright (c) 2012-2014 Bartosz Szczesny <bszcz@bszcz.org>
// This program is free software under the MIT license.

#ifndef PIXMAP_JPG_H // enables .jpg format, requires '-ljpeg' compilation flag
#define PIXMAP_JPG_H

#include "pixmap.h"
#include <jpeglib.h>

int pixmap_write_jpg(const struct pixmap* img, const char* filename);

#endif
