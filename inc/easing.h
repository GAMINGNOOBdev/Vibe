#ifndef __EASING_H_
#define __EASING_H_ 1

typedef enum
{
    easeInSine,
    easeOutSine,
    easeInOutSine,
    easeInQuad,
    easeOutQuad,
    easeInOutQuad,
    easeInCubic,
    easeOutCubic,
    easeInOutCubic,
    easeInQuart,
    easeOutQuart,
    easeInOutQuart,
    easeInQuint,
    easeOutQuint,
    easeInOutQuint,
    easeInExpo,
    easeOutExpo,
    easeInOutExpo,
    easeInCirc,
    easeOutCirc,
    easeInOutCirc,
    easeInBack,
    easeOutBack,
    easeInOutBack,
    easeInElastic,
    easeOutElastic,
    easeInOutElastic,
    easeInBounce,
    easeOutBounce,
    easeInOutBounce,
} easing_type_t;

void easing_enable(void);
void easing_disable(void);

void easing_reset_timer(void);
void easing_set_duration(float time);
void easing_set_type(easing_type_t type);
void easing_update(float delta);
float easing_get_factor(void);

#endif
