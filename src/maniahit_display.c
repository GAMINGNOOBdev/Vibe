#include <maniahit_display.h>
#include <texture.h>
#include <scoring.h>
#include <sprite.h>

#ifdef __PSP__
#include <gu2gl.h>
#else
#include <pctypes.h>
#include <cglm/cglm.h>
#endif

#include <gfx.h>

sprite_t maniahit_display_sprite;
texture_t maniahit_display_texture;
float maniahit_display_timer = 0.0f;
uint8_t maniahit_display_initialized = 0;

void maniahit_display_init(void)
{
    if (maniahit_display_initialized)
        return;

    maniahit_display_initialized = 1;

    texture_load(&maniahit_display_texture, "Skin/maniahit.png", GL_TRUE, GL_TRUE);
    sprite_create(&maniahit_display_sprite, (PSP_SCREEN_WIDTH-16)/2.f, PSP_SCREEN_HEIGHT - 48, 16, 16, &maniahit_display_texture);
}

void maniahit_display_update(float delta)
{
    if (maniahit_display_timer == 0.0f)
        return;

    maniahit_display_timer -= delta;
    if (maniahit_display_timer < 0.0f)
        maniahit_display_timer = 0.0f;
}

void maniahit_display_render()
{
    if (maniahit_display_timer == 0.0f)
        return;

    /// TODO: --- effects for rendering the hit info (smooth ease in or smth)

    sprite_draw(&maniahit_display_sprite, &maniahit_display_texture);
}

void maniahit_display_judement(scoring_judgement_type_t judgement)
{
    if (judgement == JudgementNone)
    {
        maniahit_display_timer = 0;
        return;
    }

    if (judgement == JudgementPerfect)
        sprite_change_uv(&maniahit_display_sprite, &maniahit_display_texture, 0, 0, 60, 60);
    if (judgement == JudgementGreat)
        sprite_change_uv(&maniahit_display_sprite, &maniahit_display_texture, 60, 0, 120, 60);
    if (judgement == JudgementGood)
        sprite_change_uv(&maniahit_display_sprite, &maniahit_display_texture, 120, 0, 180, 60);
    if (judgement == JudgementOk)
        sprite_change_uv(&maniahit_display_sprite, &maniahit_display_texture, 180, 0, 240, 60);
    if (judgement == JudgementMeh)
        sprite_change_uv(&maniahit_display_sprite, &maniahit_display_texture, 240, 0, 300, 60);
    if (judgement == JudgementMiss)
        sprite_change_uv(&maniahit_display_sprite, &maniahit_display_texture, 300, 0, 360, 60);

    maniahit_display_timer = 0.5f;
}

void maniahit_display_at(float x, float y, scoring_judgement_type_t judgement)
{
    maniahit_display_judement(judgement);
    maniahit_display_timer = 0.0f;

    maniahit_display_sprite.x = x;
    maniahit_display_sprite.y = y;
    sprite_draw(&maniahit_display_sprite, &maniahit_display_texture);
    maniahit_display_sprite.x = (PSP_SCREEN_WIDTH-16)/2.f;
    maniahit_display_sprite.y = PSP_SCREEN_HEIGHT - 48;
}

void maniahit_display_dispose(void)
{
    if (!maniahit_display_initialized)
        return;

    texture_dispose(&maniahit_display_texture);
    sprite_dispose(&maniahit_display_sprite);
}
