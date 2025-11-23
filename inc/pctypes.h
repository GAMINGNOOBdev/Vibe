#ifndef __PCTYPES_H_
#define __PCTYPES_H_ 1

#ifndef __PSP__

#ifndef __APPLE__
#include <GL/glew.h>
#else
#include <OpenGL/gl.h>
#endif
#include <GLFW/glfw3.h>

#define GL_MODEL GL_MODELVIEW

#define PSP_CTRL_MODE_ANALOG 0
#define PSP_CTRL_START GLFW_KEY_ENTER
#define PSP_CTRL_SELECT GLFW_KEY_SPACE
#define PSP_CTRL_CROSS GLFW_KEY_Z
#define PSP_CTRL_CIRCLE GLFW_KEY_ESCAPE
#define PSP_CTRL_TRIANGLE GLFW_KEY_S
#define PSP_CTRL_SQUARE GLFW_KEY_A
#define PSP_CTRL_UP GLFW_KEY_UP
#define PSP_CTRL_DOWN GLFW_KEY_DOWN
#define PSP_CTRL_LEFT GLFW_KEY_LEFT
#define PSP_CTRL_RIGHT GLFW_KEY_RIGHT
#define PSP_CTRL_LTRIGGER GLFW_KEY_Q
#define PSP_CTRL_RTRIGGER GLFW_KEY_W

#endif // __PSP__

#endif // __PCTYPES_H_
