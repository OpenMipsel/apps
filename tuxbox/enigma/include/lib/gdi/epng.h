#ifndef __png_h
#define __png_h

#include "grc.h"

gPixmap loadPNG(const char *filename, const gPixmap &comp);
int savePNG(const char *filename, gPixmap *pixmap);

#endif
