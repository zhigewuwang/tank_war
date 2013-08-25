#include "zfont.h"

#define GLYPH_COUNT 65535
#define TEXTURE_COUNT 10
#define FTC_CACHE_SIZE 1024 * 1024
#define DEFAULT_MAX_TEXTURE_SIZE 256

struct Texture {
	GLuint textureId;
	int left;
	int top;
};
struct Glyph {
	GLuint textureId;
	int left;
	int top;
	int width;
	int height;
};
struct FontTex {
	struct Font font;
	FT_Library lib;
	FT_Face face;
	FTC_Manager ftcManager;
	FTC_CMapCache ftcCMap;
	FTC_SBitCache ftcSBit;
	FTC_ImageTypeRec imageType;

	struct Glyph *glyphs[GLYPH_COUNT];
	struct Texture textures[TEXTURE_COUNT];
	int curTex;
	int size; // in px
	GLuint textureWidth;
	GLuint textureHeight;
	int lineHeight;
};

static void cTexture(struct FontTex *ftex, struct Texture *tex){
	unsigned char *buffer = (unsigned char *)calloc(ftex->textureWidth * ftex->textureHeight, sizeof(unsigned char));
	glGenTextures(1, &tex->textureId);
	glBindTexture(GL_TEXTURE_2D, tex->textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, ftex->textureWidth, ftex->textureHeight, 0, GL_ALPHA, GL_UNSIGNED_BYTE, buffer);
	free(buffer);
}
static FT_Error faceRequester(FTC_FaceID face_id, FT_Library library, FT_Pointer request_data, FT_Face *aface ) {
	*aface = (FT_Face)request_data;
	return 0;
}
static int cGlyph(struct FontTex *ftex, FT_UInt glyphIndex, struct Glyph **glyph) {
	FTC_SBit sbit;
	GLubyte *buffer;
	int i, j;
	struct Texture tex = ftex->textures[ftex->curTex];
	*glyph = (struct Glyph *)calloc(1, sizeof(struct Glyph));
	if (FTC_SBitCache_Lookup(ftex->ftcSBit, &ftex->imageType, glyphIndex, &sbit, 0)) return 1;
	(*glyph)->width = sbit->width;
	(*glyph)->height = sbit->height;
	buffer = (GLubyte *)malloc(sizeof(GLubyte) * sbit->width * sbit->height * 3);
	for (i = 0; i < sbit->height; i++) {
		for (j = 0; j < sbit->width; j++) {
			int si = (i * sbit->width + j) * 3;
			buffer[si] = sbit->buffer[i * sbit->width + j];
			buffer[si + 1] = sbit->buffer[i * sbit->width + j];
			buffer[si + 2] = sbit->buffer[i * sbit->width + j];
		}
	}

	if (tex.left + (*glyph)->width >= ftex->textureWidth 
			|| tex.top + ftex->lineHeight >= ftex->textureHeight) {
		ftex->curTex ++;
		cTexture(ftex, &ftex->textures[ftex->curTex]);
		tex = ftex->textures[ftex->curTex];
	}

	glBindTexture(GL_TEXTURE_2D, tex.textureId);
	glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
	glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, (*glyph)->left, (*glyph)->top, (*glyph)->width, (*glyph)->height, GL_ALPHA, GL_UNSIGNED_BYTE, sbit->buffer);
	glPopClientAttrib();

	(*glyph)->textureId = tex.textureId;
	(*glyph)->left = tex.left;
	(*glyph)->top = tex.top;
	tex.left += (*glyph)->width;
	tex.top += ftex->lineHeight;

	ftex->glyphs[glyphIndex] = *glyph;
	return 0;
}
static int rGlyph(struct FontTex *ftex, wchar_t c, struct Glyph **glyph){
	FT_UInt glyphIndex;
	glyphIndex = FTC_CMapCache_Lookup(ftex->ftcCMap, ftex->imageType.face_id, 0, c);
	*glyph = ftex->glyphs[glyphIndex];
	if (*glyph == 0) 
		if (cGlyph(ftex, glyphIndex, glyph)) 
			return 1;
	return 0;
}
static int drawChar(struct FontTex *ftex, wchar_t c, int x, int y, GLuint *lastTexId, int *width, int *height){
	struct Glyph *glyph = 0;
	float l, r, b, t, tw = ftex->textureWidth, th = ftex->textureHeight, w, h;
	if (rGlyph(ftex, c, &glyph)) return 1;
	if (*lastTexId != glyph->textureId) 
		glBindTexture(GL_TEXTURE_2D, *lastTexId = glyph->textureId);
	//printf("left : %d, top : %d, w : %d, h : %d\n", glyph->left, glyph->top, glyph->width, glyph->height);
	glBegin(GL_QUADS);
		l = 0;
		r = (float)glyph->width / ftex->textureWidth;
		b = 0;
		t = (float)glyph->height / ftex->textureHeight;
		w = glyph->width;
		h = glyph->height;
		glTexCoord2d(l, t); glVertex3i(x, y, -700); // left bottom
		glTexCoord2d(r, t); glVertex3i(x + w, y, -700); // right bottom
		glTexCoord2d(r, b); glVertex3i(x + w, y + h, -750); // right top
		glTexCoord2d(l, b); glVertex3i(x, y + h, -750); // left top
	glEnd();
	*width = w;
	*height = h;
}
static void drawText(struct Font *font, wchar_t *text, int x, int y){
	int i, len, w, h, left, top;
	GLuint lastTexId;
	struct FontTex *ftex = (struct FontTex *)font;
	for (i = 0, len = wcslen(text); i < len; i++) {
		drawChar(ftex, text[i], x, y, &lastTexId, &w, &h);
		x += w;
		if (x + ftex->size >= ftex->textureWidth) {
			x = 0;
			y -= ftex->lineHeight;
		}
	}
}
int cFont(char *fontPath, int size, struct Font **font){
	GLuint maxTextureSize = 0;
	struct FontTex *ftex = (struct FontTex *)calloc(1, sizeof(struct FontTex));
	ftex->size = size;
	if (FT_Init_FreeType(&ftex->lib)) return 1;
	if (FT_New_Face(ftex->lib, fontPath, 0, &ftex->face)) return 2;
	FT_Set_Pixel_Sizes(ftex->face, ftex->size, ftex->size);
	FTC_Manager_New(ftex->lib, 0, 0, FTC_CACHE_SIZE, faceRequester, ftex->face, &ftex->ftcManager);
	FTC_CMapCache_New(ftex->ftcManager, &ftex->ftcCMap);
	FTC_SBitCache_New(ftex->ftcManager, &ftex->ftcSBit);
	ftex->imageType.face_id = (FTC_FaceID)1;
	ftex->imageType.width = size;
	ftex->imageType.height = size;
	ftex->imageType.flags = FT_LOAD_DEFAULT;

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint *)&maxTextureSize);	
	printf("max texture size : %d\n", maxTextureSize);
	/*if (!maxTextureSize) */maxTextureSize = DEFAULT_MAX_TEXTURE_SIZE;
	ftex->textureWidth = maxTextureSize;
	ftex->textureHeight = maxTextureSize;
	ftex->lineHeight = ftex->face->size->metrics.height >> 6;
	
	cTexture(ftex, &ftex->textures[0]);

	*font = (struct Font *)ftex;
	(*font)->drawText = drawText;
	return 0;
}
void dFont(struct Font *font){
	int i;
	struct FontTex *ftex = (struct FontTex *)font;
	FTC_Manager_Done(ftex->ftcManager);
	FT_Done_FreeType(ftex->lib);
	for (i = 0; i <= ftex->curTex; i++) {
		glDeleteTextures(1, &ftex->textures[i].textureId);
	}
	free(ftex);
}
