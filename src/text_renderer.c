#include <text_renderer.h>
#include <tilemap.h>
#include <texture.h>
#include <logging.h>
#include <string.h>
#include <pspgum.h>
#include <pspgu.h>
#include <gu2gl.h>
#include <mesh.h>

#define FONT_GRID_COLS 16
#define FONT_GRID_ROWS 6
#define FONT_CHAR_START 32
#define FONT_CHAR_END 127
#define FONT_CHAR_COUNT (FONT_CHAR_END - FONT_CHAR_START)
#define FONT_CHAR_WIDTH 8
#define FONT_CHAR_HEIGHT 8

tilemap_t text_renderer_font_tilemap;
texture_t text_renderer_texture;

void text_renderer_initialize(void)
{
    texture_load(&text_renderer_texture, "Assets/font.png", GL_FALSE, GL_TRUE);
    tilemap_create(&text_renderer_font_tilemap, (texture_atlas_t){16,16}, &text_renderer_texture, 480/16, 272/16);
}

void text_renderer_draw(const char* str, float x, float y, float pixelsize)
{
    int len = strlen(str);
    memset(text_renderer_font_tilemap.tiles, 0, sizeof(tile_t)*text_renderer_font_tilemap.width*text_renderer_font_tilemap.height);

    for (int i = 0; i < len; i++)
    {
        char c = str[i];

        tile_t tile = {
            .x = i,
            .y = 0,
            .idx = c,
            .color = 0xFFFFFFFF,
        };
        //tile.y = text_renderer_font_tilemap.height - i / text_renderer_font_tilemap.width - 1;

        text_renderer_font_tilemap.tiles[i] = tile;
    }
    tilemap_build(&text_renderer_font_tilemap);

    text_renderer_font_tilemap.x = x;
    text_renderer_font_tilemap.y = y;
    text_renderer_font_tilemap.scalex = text_renderer_font_tilemap.scaley = pixelsize;

    tilemap_draw(&text_renderer_font_tilemap);
}

void text_renderer_dispose(void)
{
    tilemap_dispose(&text_renderer_font_tilemap);
}

