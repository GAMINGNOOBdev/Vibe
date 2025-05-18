#include <logging.h>
#include <easing.h>
#include <stdint.h>
#include <math.h>

uint8_t easing_enabled = 0;
float easing_timer = 0.0f;
float easing_factor = 0.0f;
float easing_duration = 0.0f;
easing_type_t easing_type = easeInSine;

void easing_enable(void)
{
    easing_enabled = 1;
}

void easing_disable(void)
{
    easing_enabled = 0;
}

void easing_reset_timer(void)
{
    easing_factor = 0;
    easing_timer = 0.0f;
}

void easing_set_duration(float time)
{
    easing_duration = time;
}

void easing_set_type(easing_type_t type)
{
    easing_type = type;
}

#define EASE_BACK_C1    1.70158f
#define EASE_BACK_C2    (EASE_BACK_C1*1.525f)
#define EASE_BACK_C3    2.70158f
#define EASE_BACK_C4    ((2.f * M_PI) / 3.f)
#define EASE_BACK_C5    ((2.f * M_PI) / 4.5f)

#define EASE_BOUNCE_N1  7.5625f
#define EASE_BOUNCE_D1  2.75f

float ease_out_bounce(float x)
{
    float x2 = x - 2.625f / EASE_BOUNCE_D1;

    if (x < 1.f / EASE_BOUNCE_D1)
        return EASE_BOUNCE_N1 * x * x;
    else if (x < 2.f / EASE_BOUNCE_D1)
    {
        x2 = x - 1.5f / EASE_BOUNCE_D1;
        return EASE_BOUNCE_N1 * x2 * x + 0.75f;
    }
    else if (x < 2.5f / EASE_BOUNCE_D1)
    {
        x2 = x - 2.25f / EASE_BOUNCE_D1;
        return EASE_BOUNCE_N1 * x2 * x + 0.9375f;
    }

    return EASE_BOUNCE_N1 * x2 * x + 0.984375f;
}

void easing_update(float delta)
{
    if (!easing_enabled)
        return;

    easing_timer += delta;
    if (easing_timer >= easing_duration)
        easing_timer = easing_duration;
}

float easing_get_factor(void)
{
    if (!easing_enabled)
        return 1;

    float x = easing_timer / easing_duration;
    if (x == 1.f)
        return 1;

    if (easing_type == easeInSine)
        easing_factor = 1.f - cosf((x * M_PI) / 2.f);
    else if (easing_type == easeOutSine)
        easing_factor = sinf((x * M_PI) / 2.f);
    else if (easing_type == easeInOutSine)
        easing_factor = -(cosf(M_PI * x) - 1.f) / 2.f;
    else if (easing_type == easeInQuad)
        easing_factor = x*x;
    else if (easing_type == easeOutQuad)
        easing_factor = 1.f - (1.f - x) * (1.f - x);
    else if (easing_type == easeInOutQuad)
        easing_factor = x < 0.5f ? 2.f * x*x : 1.f - powf(-2.f * x + 2.f, 2.f) / 2.f;
    else if (easing_type == easeInCubic)
        easing_factor = x*x*x;
    else if (easing_type == easeOutCubic)
        easing_factor = 1.f - powf(1.f - x, 3.f);
    else if (easing_type == easeInOutCubic)
        easing_factor = x < 0.5f ? 4.f * x*x*x : 1.f - powf(-2.f * x + 2.f, 3.f) / 2.f;
    else if (easing_type == easeInQuart)
        easing_factor = x*x*x*x;
    else if (easing_type == easeOutQuart)
        easing_factor = 1.f - powf(1.f - x, 4.f);
    else if (easing_type == easeInOutQuart)
        easing_factor = x < 0.5f ? 8.f * x*x*x*x : 1.f - powf(-2.f * x + 2.f, 4.f) / 2.f;
    else if (easing_type == easeInQuint)
        easing_factor = x*x*x*x*x;
    else if (easing_type == easeOutQuint)
        easing_factor = 1.f - powf(1.f - x, 5.f);
    else if (easing_type == easeInOutQuint)
        easing_factor = x < 0.5f ? 16.f * x*x*x*x*x : 1.f - powf(-2.f * x + 2.f, 5.f) / 2.f;
    else if (easing_type == easeInExpo)
        easing_factor = x == 0.f ? 0.f : powf(2.f, 10.f * x - 10.f);
    else if (easing_type == easeOutExpo)
        easing_factor = x == 1.f ? 1.f : 1.f - powf(2.f, -10.f * x);
    else if (easing_type == easeInOutExpo)
        easing_factor = x == 0.f ? 0 : x == 1.f ? 1.f : x < 0.5f ? powf(2.f, 20.f * x - 10.f) / 2.f : (2.f - powf(2.f, -20.f * x + 10.f)) / 2.f;
    else if (easing_type == easeInCirc)
        easing_factor = 1.f - sqrtf(1.f - powf(x, 2.f));
    else if (easing_type == easeOutCirc)
        easing_factor = sqrtf(1.f - powf(x - 1.f, 2.f));
    else if (easing_type == easeInOutCirc)
        easing_factor = x < 0.5f ? (1.f - sqrtf(1.f - powf(2.f * x, 2.f))) / 2.f : (sqrtf(1.f - powf(-2.f * x + 2.f, 2.f)) + 1.f) / 2.f;
    else if (easing_type == easeInBack)
        easing_factor = EASE_BACK_C3 * x*x*x - EASE_BACK_C1 * x*x;
    else if (easing_type == easeOutBack)
        easing_factor = 1.f + EASE_BACK_C3 * powf(x - 1.f, 3.f) + EASE_BACK_C1 * powf(x - 1.f, 2.f);
    else if (easing_type == easeInOutBack)
        easing_factor = x < 0.5f ? (powf(2.f * x, 2.f) * ((EASE_BACK_C2 + 1.f) * 2.f * x - EASE_BACK_C2)) / 2.f : (powf(2.f * x - 2.f, 2.f) * ((EASE_BACK_C2 + 1.f) * (x * 2.f - 2.f) + EASE_BACK_C2) + 2.f) / 2.f;
    else if (easing_type == easeInElastic)
        easing_factor = x == 0.f ? 0.f : x == 1.f ? 1.f : -powf(2.f, 10.f * x - 10.f) * sinf((x * 10.f - 10.75f) * EASE_BACK_C4);
    else if (easing_type == easeOutElastic)
        easing_factor = x == 0.f ? 0.f : x == 1.f ? 1.f : powf(2.f, -10.f * x) * sinf((x * 10.f - 0.75f) * EASE_BACK_C4) + 1.f;
    else if (easing_type == easeInOutElastic)
        easing_factor = x == 0.f ? 0.f : x == 1.f ? 1.f : x < 0.5f ? -(powf(2.f, 20.f * x - 10.f) * sinf((20.f * x - 11.125f) * EASE_BACK_C5)) / 2.f : (powf(2.f, -20.f * x + 10.f) * sinf((20.f * x - 11.125f) * EASE_BACK_C5)) / 2.f + 1.f;
    else if (easing_type == easeInBounce)
        easing_factor = 1.f - ease_out_bounce(1.f - x);
    else if (easing_type == easeOutBounce)
        easing_factor = ease_out_bounce(x);
    else if (easing_type == easeInOutBounce)
        easing_factor = x < 0.5f ? (1.f - ease_out_bounce(1.f - 2.f*x)) / 2.f : (1.f + ease_out_bounce(2.f*x - 1.f)) / 2.f;

    return easing_factor;
}
