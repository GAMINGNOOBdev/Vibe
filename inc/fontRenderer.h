#ifndef __FONTRENDERER_H_
#define __FONTRENDERER_H_ 1

#include <fontFile.h>
#include <texture.h>
#include <mesh.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint16_t id;
    uint16_t advanceX;
    float x0, x1;
    float y0, y1;
    float u0, u1;
    float v0, v1;
} fontChar_t;

typedef struct
{
    fontFile_t* fontFile;
    texture_t* texture;
    mesh_t* mesh;
    float lineSpacing;
    float pixelScale;
    float scale;
} fontRenderer_t;

uint8_t fontGetBakedChar(uint16_t id, uint16_t xPos, uint16_t yPos, float scale, float pxScale, fontChar_t* result, fontFileHeader_t* header, fontCharacterGlyphList_t* glyphs);

fontRenderer_t* createFontRenderer(const char* fontPath, const char* fontFileName, float pxHeight, float lineSpacing);
void fontRendererBuildText(fontRenderer_t* renderer, const char* text, uint16_t x, uint16_t y, uint32_t color);
void fontRendererDraw(fontRenderer_t* renderer);
void disposeFontRenderer(fontRenderer_t* renderer);

#ifdef __cplusplus
}
#endif

#endif