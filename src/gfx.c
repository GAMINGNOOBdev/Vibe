#define GUGL_IMPLEMENTATION
#include <gfx.h>
#ifdef __PSP__
#include <gu2gl.h>
#else
#include <GL/glew.h>
#include <GL/gl.h>
#include <input.h>
#include <memory.h>
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
#include <miniz.h>
#include <file_util.h>
#include <callback.h>
#include <logging.h>
#include <GL/glew.h>
#include <pctypes.h>
#include <GLFW/glfw3.h>
#include <sys/stat.h>
#include <song_list.h>

GLFWwindow* glfwwindow;
GLuint globalShader;
GLint shaderProjectionID, shaderModelID, shaderTextureID, shaderTextureAvailableID;

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
        LOGERROR("Shader compilation error: %s", message);
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
        LOGERROR("Shader linking error: %s", message);
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

void graphicsWindowResizeEvent(GLFWwindow* win, int width, int height)
{
    LOGINFO("Resized window??? %dx%d", width, height);
}

extern int last_pressed_key;
extern uint8_t input_locked;
void graphicsWindowKeyboardEvent(GLFWwindow* win, int key, int scancode, int action, int _)
{
    if (input_locked)
        return;

    last_pressed_key = -1;
    if (action == GLFW_PRESS)
        last_pressed_key = key;

    input_write(key, action);
}

void graphicsWindowMouseButtonEvent(GLFWwindow* win, int button, int action, int _)
{
    int width, height;
    glfwGetWindowSize(win, &width, &height);

    double xpos, ypos;
    glfwGetCursorPos(win, &xpos, &ypos);

    double scaleFactorX = (double)PSP_SCREEN_WIDTH / width;
    double scaleFactorY = (double)PSP_SCREEN_HEIGHT / height;

    if (action == GLFW_PRESS)
    {
        int x = xpos * scaleFactorX;
        int y = ypos * scaleFactorY;

        LOGDEBUG("pressed mouse at psp position %d|%d (inv y: %d|%d) or windowpos %2.2f|%2.2f", x, y, x, PSP_SCREEN_HEIGHT-y, xpos, ypos);
    }
}

void graphicsWindowMouseMoveEvent(GLFWwindow* win, double xPos, double yPos)
{
}

void graphicsWindowMouseScrollEvent(GLFWwindow* win, double xScroll, double yScroll)
{
}

void extractZipFileToDest(const char* filepath)
{
    mz_zip_archive archive = {};
    if (!mz_zip_reader_init_file(&archive, filepath, 0))
    {
        LOGERROR("Unable to extract file '%s'", filepath);
        return;
    }

    char* outputdir = "Songs/";
    const char* dir = &filepath[strlpos(filepath, '/')+1];
    size_t size =  strlen(dir) - 3;
    outputdir = malloc(strlen("Songs/") + size);
    memset(outputdir, 0, strlen("Songs/") + size);
    strncpy(outputdir, "Songs/", strlen("Songs/"));
    strncpy(&outputdir[strlen("Songs/")], dir, strlen(dir) - 2);
    LOGERROR("outputdir: '%s'", outputdir);

    const int mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
    mkdir(outputdir, mode);

    int file_count = mz_zip_reader_get_num_files(&archive);
    for (int i = 0; i < file_count; i++)
    {
        mz_zip_archive_file_stat file;
        if (!mz_zip_reader_file_stat(&archive, i, &file))
        {
            LOGERROR("Unable to extract file %d", i);
            break;
        }

        if (mz_zip_reader_is_file_a_directory(&archive, i))
        {
            LOGINFO("DIRECTORY '%s/%s'", outputdir, file.m_filename);
            mkdir(stringf("%s/%s", outputdir, file.m_filename), mode);
            continue;
        }

        LOGINFO("extracting '%s/%s'", outputdir, file.m_filename);
        if (!mz_zip_reader_extract_to_file(&archive, i, stringf("%s/%s", outputdir, file.m_filename), 0))
        {
            LOGERROR("failed to extract file %s", file.m_filename);
        }
    }

    song_list_initialize("Songs");
}

void graphicsWindowFileDropEvent(GLFWwindow* win, int path_count, const char** paths)
{
    for (int i = 0; i < path_count; i++)
    {
        int idx = strlpos(paths[i], '.');
        if (idx == -1)
            continue;
    
        if (strcmp(&paths[i][idx], ".osz") != 0)
            continue;

        LOGINFO("Extracting '%s'...", paths[i]);
        extractZipFileToDest(paths[i]);
    }
}

void graphics_init()
{
    int glfwstatus = glfwInit();
    if (glfwstatus != GLFW_TRUE)
    {
        LOGERROR("Failed to initialize glfw");
        stop_running();
        return;
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwwindow = glfwCreateWindow(PSP_SCREEN_WIDTH*2, PSP_SCREEN_HEIGHT*2, "psp-game-desktop-client", NULL, NULL);
    glfwMakeContextCurrent(glfwwindow);
    glfwFocusWindow(glfwwindow);
    glfwSwapInterval(1); // 1 = vsync, 0 = no vsync

    glfwSetFramebufferSizeCallback(glfwwindow, graphicsWindowResizeEvent);
    glfwSetKeyCallback(glfwwindow, graphicsWindowKeyboardEvent);
    glfwSetMouseButtonCallback(glfwwindow, graphicsWindowMouseButtonEvent);
    glfwSetCursorPosCallback(glfwwindow, graphicsWindowMouseMoveEvent);
    glfwSetScrollCallback(glfwwindow, graphicsWindowMouseScrollEvent);
    glfwSetDropCallback(glfwwindow, graphicsWindowFileDropEvent);

    GLenum error;
    if ((error = glewInit()) != GLEW_OK)
    {
        LOGERROR("Couldn't init glew smh: %s", glewGetErrorString(error));
        glfwTerminate();
        stop_running();
        return;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugCallback, 0);

    void* vertexshader = file_util_file_contents("Assets/desktop.vs");
    void* pixelshader = file_util_file_contents("Assets/desktop.fs");
    globalShader = create_shader(vertexshader, pixelshader);
    free(vertexshader);
    free(pixelshader);

    shaderModelID = glGetUniformLocation(globalShader, "u_modelview");
    shaderTextureID = glGetUniformLocation(globalShader, "u_texture");
    shaderProjectionID = glGetUniformLocation(globalShader, "u_projection");
    shaderTextureAvailableID = glGetUniformLocation(globalShader, "u_texture_available");

    LOGINFO("Shader IDs{ .projection=%d, .model=%d, .texture=%d, .textureAvailable=%d }", shaderProjectionID, shaderModelID, shaderTextureID, shaderTextureAvailableID);
}

void graphics_dispose()
{
    glfwDestroyWindow(glfwwindow);
    glfwTerminate();
}

void graphics_start_frame()
{
    int width, height;
    glfwGetWindowSize(glfwwindow, &width, &height);
    glViewport(0, 0, width, height);
    glUseProgram(globalShader);
}

void graphics_end_frame()
{
    glUseProgram(0);
    glfwSwapBuffers(glfwwindow);
    glfwPollEvents();
}

uint8_t graphics_should_terminate(void)
{
    return glfwWindowShouldClose(glfwwindow);
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

void graphics_no_texture(void)
{
    glUniform1ui(shaderTextureAvailableID, 0);
}

void graphics_texture_uniform(GLuint id)
{
    glUniform1ui(shaderTextureAvailableID, 1);
    glUniform1i(shaderTextureID, id);
}

#endif
