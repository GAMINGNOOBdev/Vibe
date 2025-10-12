#ifndef __GAMING_H_
#define __GAMING_H_ 1

#include <texture.h>
#include <sprite.h>
#include <audio.h>

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

typedef struct
{
    audio_stream_t drum_hitclap, drum_hitnormal, drum_hitfinish, drum_hitwhistle;
    audio_stream_t soft_hitclap, soft_hitnormal, soft_hitfinish, soft_hitwhistle;
    audio_stream_t normal_hitclap, normal_hitnormal, normal_hitfinish, normal_hitwhistle;
} gaming_soundinfo_t;

void switch_to_gaming(const char* beatmap_folder, const char* beatmap_path);

void gaming_init(void);
void gaming_dispose(void);
void gaming_update(float delta);
void gaming_render(void);

#endif
