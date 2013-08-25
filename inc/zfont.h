#ifndef ZFONT_H
#define ZFONT_H
#include "zgl.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H


struct Font {
	void (*drawText)(struct Font *font, wchar_t *text, int x, int y);	
} Font;

int cFont(char *fontPath, int size, struct Font **font);
void dFont(struct Font *font);


#endif
