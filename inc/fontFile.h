#ifndef __FONTFILE_H_
#define __FONTFILE_H_ 1

#include <stdint.h>
#include <memory.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    // Height of each line
    uint16_t lineHeight;

    // Font pixel size
    uint16_t size;

    // Width of the font bitmap
    uint16_t width;

    // Height of the font bitmap
    uint16_t height;
    
    // Font bitmap path
    char* image;

    // Font character spacing
    uint16_t spacingX, spacingY;

    // Font character padding
    uint16_t padL, padR, padU, padD;
} fontFileHeader_t;

struct fontCharacterGlyph_t;
typedef struct fontCharacterGlyph_t
{
    struct fontCharacterGlyph_t* prev;

    // Character ID
    uint16_t id;

    // X Position of this glyph
    uint16_t x;

    // Y Position of this glyph
    uint16_t y;

    // Width of this glyph
    uint16_t width;

    // Height of this glyph
    uint16_t height;

    // X Offset of this glyph
    uint16_t offX;

    // X Offset of this glyph
    uint16_t offY;

    // x advanced in pixels
    uint16_t advanceX;

    struct fontCharacterGlyph_t* next;
} fontCharacterGlyph_t;

typedef struct
{
    fontCharacterGlyph_t* start;
    fontCharacterGlyph_t* end;
    size_t count;
} fontCharacterGlyphList_t;

typedef struct
{
    fontCharacterGlyphList_t* glyphs;
    fontFileHeader_t* header;
} fontFile_t;

fontFileHeader_t* createFontFileHeader();
void disposeFontFileHeader(fontFileHeader_t* header);

fontCharacterGlyphList_t* createFontCharacterGlyphList();
void disposeFontCharacterGlyphList(fontCharacterGlyphList_t* list);
fontCharacterGlyph_t* addFontCharacterGlyphListEntry(fontCharacterGlyphList_t* list);

fontFile_t* loadFontFile(const char* path);
void disposeFontFile(fontFile_t* file);

#ifdef __cplusplus
}
#endif

#endif