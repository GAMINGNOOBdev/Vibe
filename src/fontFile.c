#include <fontFile.h>
#include <logging.h>
#include <malloc.h>
#include <stdio.h>

fontFileHeader_t* createFontFileHeader()
{
    fontFileHeader_t* header = malloc(sizeof(fontFileHeader_t));
    if (header != NULL)
        memset(header, 0, sizeof(fontFileHeader_t));
    return header;
}

void disposeFontFileHeader(fontFileHeader_t* header)
{
    if (header == NULL)
        return;

    if (header->image != NULL)
        free(header->image);

    free(header);
}

fontCharacterGlyphList_t* createFontCharacterGlyphList()
{
    fontCharacterGlyphList_t* list = malloc(sizeof(fontCharacterGlyphList_t));
    if (list != NULL)
        memset(list, 0, sizeof(fontCharacterGlyphList_t));
    return list;
}

void disposeFontCharacterGlyphList(fontCharacterGlyphList_t* list)
{
    if (list == NULL)
        return;

    for (fontCharacterGlyph_t* entry = list->start; entry != NULL;)
    {
        fontCharacterGlyph_t* next = entry->next;
        free(entry);
        entry = next;
    }

    free(list);
}

fontCharacterGlyph_t* addFontCharacterGlyphListEntry(fontCharacterGlyphList_t* list)
{
    if (list == NULL)
        return NULL;

    fontCharacterGlyph_t* entry = malloc(sizeof(fontCharacterGlyph_t));
    if (entry == NULL)
        return NULL;

    memset(entry, 0, sizeof(fontCharacterGlyph_t));

    if (list->start == NULL)
    {
        list->start = list->end = entry;
        list->count++;
        return entry;
    }

    entry->prev = list->end;
    list->end->next = entry;
    list->end = entry;
    list->count++;
    return entry;
}

void fontFileReadGlyph(FILE* file, fontCharacterGlyphList_t* list)
{
    if (file == NULL || list == NULL)
        return;

    fontCharacterGlyph_t* glyph = addFontCharacterGlyphListEntry(list);
    if (glyph == NULL)
        return;

    fread(&glyph->id, sizeof(uint16_t), 1, file);
    fread(&glyph->x, sizeof(uint16_t), 1, file);
    fread(&glyph->y, sizeof(uint16_t), 1, file);
    fread(&glyph->width, sizeof(uint16_t), 1, file);
    fread(&glyph->height, sizeof(uint16_t), 1, file);
    fread(&glyph->offX, sizeof(uint16_t), 1, file);
    fread(&glyph->offY, sizeof(uint16_t), 1, file);
    fread(&glyph->advanceX, sizeof(uint16_t), 1, file);

    LOGDEBUG(stringf("glyph{ 0x%8.8x | pos: %d %d | size: %dx%d | offset: %d %d | advX: %d}", 
                      glyph->id, glyph->x, glyph->y, glyph->width, glyph->height, glyph->offX, glyph->offY, glyph->advanceX));
}

fontFile_t* loadFontFile(const char* path)
{
    fontFile_t* font = malloc(sizeof(fontFile_t));
    if (font == NULL)
        return NULL;

    memset(font, 0, sizeof(fontFile_t));
    font->header = createFontFileHeader();
    if (font->header == NULL)
    {
        free(font);
        return NULL;
    }
    font->glyphs = createFontCharacterGlyphList();
    if (font->glyphs == NULL)
    {
        disposeFontFileHeader(font->header);
        free(font);
        return NULL;
    }

    FILE* file = fopen(path, "rb");

    fread(&font->header->lineHeight, sizeof(uint16_t), 1, file);
    fread(&font->header->size, sizeof(uint16_t), 1, file);
    fread(&font->header->width, sizeof(uint16_t), 1, file);
    fread(&font->header->height, sizeof(uint16_t), 1, file);
    fread(&font->header->spacingX, sizeof(uint16_t), 1, file);
    fread(&font->header->spacingY, sizeof(uint16_t), 1, file);
    fread(&font->header->padL, sizeof(uint16_t), 1, file);
    fread(&font->header->padR, sizeof(uint16_t), 1, file);
    fread(&font->header->padU, sizeof(uint16_t), 1, file);
    fread(&font->header->padD, sizeof(uint16_t), 1, file);
    uint8_t strLength = 0;
    fread(&strLength, sizeof(uint8_t), 1, file);
    font->header->image = malloc(strLength+1);
    if (font->header->image == NULL)
    {
        disposeFontCharacterGlyphList(font->glyphs);
        disposeFontFileHeader(font->header);
        free(font);
        return NULL;
    }
    memset(font->header->image, 0, strLength+1);
    fread(font->header->image, 1, strLength, file);

    uint16_t glyphCount = 0;
    fread(&glyphCount, sizeof(uint16_t), 1, file);
    for (uint16_t i = 0; i < glyphCount; i++)
        fontFileReadGlyph(file, font->glyphs);

    fclose(file);
    return font;
}

void disposeFontFile(fontFile_t* font)
{
    if (font == NULL)
        return;

    disposeFontCharacterGlyphList(font->glyphs);
    disposeFontFileHeader(font->header);
    free(font);
}