#ifndef __TEXT_RENDERER_H_
#define __TEXT_RENDERER_H_ 1

void text_renderer_initialize(void);
void text_renderer_draw(const char* str, float x, float y, float pixelsize);
void text_renderer_dispose(void);

#endif
