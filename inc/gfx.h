#ifndef __GFX_H_
#define __GFX_H_ 1

#ifdef __PSP__
#include <pspdisplay.h>
#include <pspgum.h>
#include <pspgu.h>
#else
#include <GL/glew.h>
#include <cglm/cglm.h>
#endif

#define PSP_BUFFER_WIDTH 512
#define PSP_SCREEN_WIDTH 480
#define PSP_SCREEN_HEIGHT 272

void graphics_init();
void graphics_dispose();
void graphics_start_frame();
void graphics_end_frame();

#ifndef __PSP__
uint8_t graphics_should_terminate(void);
void graphics_projection_matrix(mat4 matrix);
void graphics_model_matrix(mat4 matrix);
void graphics_no_texture(void);
void graphics_texture_uniform(GLuint id);
#endif

#endif
