#ifndef __GAMING_H_
#define __GAMING_H_ 1

#include <texture.h>
#include <sprite.h>

typedef struct
{
    texture_t judgementline_texture;
    texture_t long_note_texture;
    texture_t lanehit_texture;
    texture_t note1_texture;
    texture_t note2_texture;

    texture_t maniahit300g_texture;
    texture_t maniahit300_texture;
    texture_t maniahit200_texture;
    texture_t maniahit100_texture;
    texture_t maniahit50_texture;
    texture_t maniahit0_texture;

    sprite_t long_note;
    sprite_t maniahit;
    sprite_t note;
} gaming_drawinfo_t;

void switch_to_gaming(const char* beatmap_folder, const char* beatmap_path);

void gaming_init(void);
void gaming_dispose(void);
void gaming_update(float delta);
void gaming_render(void);

#endif
