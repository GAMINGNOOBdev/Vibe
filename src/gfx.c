#define GUGL_IMPLEMENTATION
#include <gfx.h>
#ifdef __PSP__
#include <gu2gl.h>
#else
#include <SDL2/SDL_video.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <pctypes.h>
#endif

#ifdef __PSP__

static uint32_t __attribute__((aligned(16))) guDrawList[262144];

void graphics_init()
{
    guglInit(guDrawList);
}

void graphics_dispose()
{
    guglTerm();
}

void graphics_start_frame()
{
    guglStartFrame(guDrawList, GL_FALSE);
}

void graphics_end_frame()
{
    guglSwapBuffers(GL_TRUE, GL_FALSE);
}

#else
#include <file_util.h>
#include <callback.h>
#include <logging.h>
#include <GL/glew.h>
#include <pctypes.h>

SDL_Window* sdlwindow;
GLuint globalSdlShader;
SDL_GLContext glcontext;
GLint shaderProjectionID, shaderModelID, shaderTextureID;

GLuint compile_shader(const char* src, GLenum type)
{
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &src, 0);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE)
    {
        int length = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = malloc(length+1);
        memset(message, 0, length+1);
        glGetShaderInfoLog(id, length, &length, message);
        LOGERROR(stringf("Shader compilation error: %s", message));
        free(message);
        return 0;
    }
    return id;
}

GLuint create_shader(const char* vertexShader, const char* fragmentShader)
{
    GLuint program = glCreateProgram();
    GLuint vs = compile_shader(vertexShader, GL_VERTEX_SHADER);
    GLuint fs = compile_shader(fragmentShader, GL_FRAGMENT_SHADER);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int result;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* message = malloc(length+1);
        memset(message, 0, length+1);
        glGetProgramInfoLog(program, length, &length, message);
        LOGERROR(stringf("Shader linking error: %s", message));
        free(message);
        return 0;
    }

    glDetachShader(program, vs);
    glDetachShader(program, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

//////////////////////////////////////
/// Debugging and helper functions ///
//////////////////////////////////////

const char* glDebugTypeString(GLenum type)
{
    if (type == GL_DEBUG_TYPE_ERROR)
        return "ERROR";

    if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
        return "DEPRECATED BEHAVIOR";

    if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
        return "UNDEFINED BEHAVIOR";

    if (type == GL_DEBUG_TYPE_PORTABILITY)
        return "PORTABILITY";

    if (type == GL_DEBUG_TYPE_PERFORMANCE)
        return "PERFORMANCE";

    return "OTHER";
}

const char* glDebugSeverityString(GLenum severity)
{
    if (severity == GL_DEBUG_SEVERITY_HIGH)
        return "HIGH";

    if (severity == GL_DEBUG_SEVERITY_MEDIUM)
        return "MEDIUM";

    return "LOW";
}

void GLAPIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if (severity == GL_DEBUG_SEVERITY_LOW)
        return;
    
    if (type == GL_DEBUG_TYPE_OTHER)
        return;

    LOG(type == GL_DEBUG_TYPE_ERROR ? LOGLEVEL_ERROR : LOGLEVEL_DEBUG, stringf("[%s][%s]: %s", glDebugSeverityString(severity), glDebugTypeString(type), message));
}

void graphics_init()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_GL_SetSwapInterval(0);

    sdlwindow = SDL_CreateWindow("psp-game-client-desktop", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, PSP_SCREEN_WIDTH*2, PSP_SCREEN_HEIGHT*2, SDL_WINDOW_OPENGL);
    glcontext = SDL_GL_CreateContext(sdlwindow);

    GLenum error;
    if ((error = glewInit()) != GLEW_OK)
    {
        LOGERROR(stringf("Couldn't init glew smh: %s", glewGetErrorString(error)));
        SDL_Quit();
        stop_running();
        return;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugCallback, 0);

    void* vertexshader = file_util_file_contents("Assets/desktop.vs");
    void* pixelshader = file_util_file_contents("Assets/desktop.fs");
    globalSdlShader = create_shader(vertexshader, pixelshader);
    free(vertexshader);
    free(pixelshader);

    shaderModelID = glGetUniformLocation(globalSdlShader, "u_modelview");
    shaderTextureID = glGetUniformLocation(globalSdlShader, "u_texture");
    shaderProjectionID = glGetUniformLocation(globalSdlShader, "u_projection");

    LOGINFO(stringf("Shader IDs{ .projection=%d, .model=%d, .texture=%d }", shaderProjectionID, shaderModelID, shaderTextureID));
}

void graphics_dispose()
{
    SDL_GL_DeleteContext(glcontext);
    SDL_Quit();
}

void graphics_start_frame()
{
    glViewport(0, 0, PSP_SCREEN_WIDTH*2, PSP_SCREEN_HEIGHT*2);
    glUseProgram(globalSdlShader);
}

void graphics_end_frame()
{
    glUseProgram(0);
    SDL_GL_SwapWindow(sdlwindow);
}

void graphics_projection_matrix(mat4 matrix)
{
    float data[16];
    for (uint8_t y = 0; y < 4; y++)
        for (uint8_t x = 0; x < 4; x++)
            data[y*4+x] = matrix[y][x];

    glUniformMatrix4fv(shaderProjectionID, 1, 0, data);
}

void graphics_model_matrix(mat4 matrix)
{
    float data[16];
    for (uint8_t y = 0; y < 4; y++)
        for (uint8_t x = 0; x < 4; x++)
            data[y*4+x] = matrix[y][x];

    glUniformMatrix4fv(shaderModelID, 1, 0, data);
}

void graphics_texture_uniform(GLuint id)
{
    glUniform1i(shaderTextureID, id);
}

#endif
