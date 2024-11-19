#include <fontRenderer.h>
#include <pspkernel.h>
#include <logging.h>
#include <vertex.h>
#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include <gu2gl.h>

uint8_t fontGetBakedChar(uint16_t id, uint16_t xPosition, uint16_t yPosition, float scale, float pxScale, fontChar_t* result, fontFileHeader_t* header, fontCharacterGlyphList_t* glyphs)
{
    /**
     * "Find" the character
     */
    fontCharacterGlyph_t* glyph = NULL;

    for (fontCharacterGlyph_t* entry = glyphs->start; entry != NULL; entry = entry->next)
    {
        if (entry->id == id)
        {
            glyph = entry;
            break;
        }
    }

    if (glyph == NULL)
        return 0;

    /**
     * Calculate vertex positions and stuff
     */
    float x, y, width, height;
    float texWidth, texHeight;
    float displayWidth, displayHeight;

    texWidth = (float)header->width;
    texHeight = (float)header->height;

    result->id = glyph->id;

    x = glyph->x;
    y = glyph->y;
    width = glyph->width;
    height = glyph->height;

    float aspect = width / height;
    float xPos = (float)xPosition/* + glyph->OffsetX / width * scale * pxScale*/;
    float yPos = (float)yPosition/* + glyph->OffsetY / height * scale * pxScale*/;

    displayWidth = pxScale * aspect * scale;
    displayHeight = pxScale * scale;

    float u0 = x / texWidth;
    float u1 = (x+width) / texWidth;
    float v0 = y / texHeight;
    float v1 = (y+height) / texHeight;

    float x0 = xPos;
    float x1 = xPos + displayWidth;
    float y0 = yPos;
    float y1 = yPos + displayHeight;

    result->advanceX = (uint16_t)(((float)glyph->advanceX) / height * pxScale * scale);
    result->x0 = x0;
    result->x1 = x1;
    result->y0 = y0;
    result->y1 = y1;
    result->u0 = u0;
    result->u1 = u1;
    result->v0 = v0;
    result->v1 = v1;

    LOGWARNING(stringf("GET BAKED CHAR '0x%8.8x'(0x%8.8x): posxy{%f|%f} xy{%f|%f} wh{%f|%f} texSz{%f x %f} dispSz{%f x %f} advX{%d | og: %d} uv0{%f|%f} uv1{%f|%f} xy0{%f|%f} xy1{%f|%f}",
             id, glyph->id, xPos, yPos, x, y, width, height,
             texWidth, texHeight, displayWidth, displayHeight,
             result->advanceX, glyph->advanceX,
             u0,v0,u1,v1,x0,y0,x1,y1));

    return 1;
}

fontRenderer_t* createFontRenderer(const char* fontPath, const char* fontFileName, float pxHeight, float lineSpacing)
{
    fontRenderer_t* renderer = malloc(sizeof(fontRenderer_t));
    if (renderer == NULL)
        return NULL;

    memset(renderer, 0, sizeof(fontRenderer_t));

    renderer->pixelScale = pxHeight;
    renderer->lineSpacing = lineSpacing;
    renderer->scale = 1.0f;

    renderer->fontFile = loadFontFile(stringf("%s/%s", fontPath, fontFileName));
    if (renderer->fontFile == NULL)
    {
        free(renderer);
        return NULL;
    }

    size_t pathLen = strlen(fontPath)+strlen(renderer->fontFile->header->image)+1;
    char* path = malloc(pathLen+1);
    if (path == NULL)
    {
        disposeFontFile(renderer->fontFile);
        free(renderer);
        return NULL;
    }
    memset(path, 0, pathLen+1);
    memcpy(path, stringf("%s/%s", fontPath, renderer->fontFile->header->image), pathLen);
    renderer->texture = loadTexture(path, GL_TRUE, GL_TRUE);
    free(path);
    if (renderer->texture == NULL)
    {
        disposeFontFile(renderer->fontFile);
        free(renderer);
        return NULL;
    }


    return renderer;
}

float fontWhitespaceAdvance(uint32_t c, float mScale, float mPixelScale, fontFile_t* font)
{
    if (font == NULL)
        return 0;

    if (c != ' ' && c != '\t')
        return 0;

    fontChar_t bakedChr;
    fontGetBakedChar(' ', 0, 0, mScale, mPixelScale, &bakedChr, font->header, font->glyphs);

    if (c == ' ')
        return bakedChr.advanceX;

    if (c == '\t')
        return bakedChr.advanceX * 4;

    return 0;
}

void fontRendererBuildText(fontRenderer_t* renderer, const char* text, uint16_t x, uint16_t y, uint32_t color)
{
    if (renderer == NULL || text == NULL)
        return;

    int text_len = strlen(text);
    uint16_t X = x, Y = y;

    if (renderer->mesh != NULL)
        disposeMesh(renderer->mesh);

    renderer->mesh = createMesh(text_len*4, text_len*6);

    fontChar_t bakedChr;
    for (int i = 0; i < text_len; i++)
    {
        uint32_t c = text[i];

        if (c == ' ' || c == '\t')
        {
            X += fontWhitespaceAdvance(c, renderer->scale, renderer->pixelScale, renderer->fontFile);
            continue;
        }

        if (c == '\n')
        {
            X = 0;
            Y += renderer->pixelScale * (1 + renderer->lineSpacing) * renderer->scale;
            continue;
        }

        if (!fontGetBakedChar(c, X, Y, renderer->scale, renderer->pixelScale, &bakedChr, renderer->fontFile->header, renderer->fontFile->glyphs))
            continue;

        LOGDEBUG(stringf("backed char '%c': uv0{%f|%f} uv1{%f|%f} xy0{%f|%f} xy1{%f|%f} advanceX{%d}}", c,
                         bakedChr.u0, bakedChr.v0, bakedChr.u1, bakedChr.v1,
                         bakedChr.x0, bakedChr.y0, bakedChr.x1, bakedChr.y1,
                         bakedChr.advanceX));

        ((vertex_t*)renderer->mesh->data)[(i * 4) + 0] = createVertex(bakedChr.u0, bakedChr.v0, color, bakedChr.x0, bakedChr.y0, 0);
        ((vertex_t*)renderer->mesh->data)[(i * 4) + 1] = createVertex(bakedChr.u0, bakedChr.v1, color, bakedChr.x0, bakedChr.y1, 0);
        ((vertex_t*)renderer->mesh->data)[(i * 4) + 2] = createVertex(bakedChr.u1, bakedChr.v1, color, bakedChr.x1, bakedChr.y1, 0);
        ((vertex_t*)renderer->mesh->data)[(i * 4) + 3] = createVertex(bakedChr.u1, bakedChr.v0, color, bakedChr.x1, bakedChr.y0, 0);

        renderer->mesh->indices[(i * 6) + 0] = 0;
        renderer->mesh->indices[(i * 6) + 1] = 1;
        renderer->mesh->indices[(i * 6) + 2] = 2;

        renderer->mesh->indices[(i * 6) + 3] = 2;
        renderer->mesh->indices[(i * 6) + 4] = 3;
        renderer->mesh->indices[(i * 6) + 5] = 0;

        X += bakedChr.advanceX;
    }

    sceKernelDcacheWritebackInvalidateAll();

    ///TODO: --- fix font renderer not creating right vertex data for some reason ---
}

void fontRendererDraw(fontRenderer_t* renderer)
{
    if (renderer == NULL)
        return;

    if (renderer->mesh == NULL)
        return;

    if (renderer->mesh->indexCount == 0)
        return;

    bindTexture(renderer->texture);
    drawMesh(renderer->mesh);
}

void disposeFontRenderer(fontRenderer_t* renderer)
{
    if (renderer == NULL)
        return;

    disposeFontFile(renderer->fontFile);
    disposeTexture(renderer->texture);
    disposeMesh(renderer->mesh);
}