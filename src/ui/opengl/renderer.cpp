#include "SDLMaster.h"
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "renderer.h"
#include "glapi.h"
#include "io/image.h"


#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049
#define TEXTURE_FREE_MEMORY_ATI 0x87FC

bool Renderer::s_symbolsLoaded = false;


bool Renderer::StaticInit()
{
    s_symbolsLoaded = false;

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1); // Share objects between GL contexts

    if (SDL_GL_LoadLibrary(NULL) == -1)
        return false;

    return true;
}

void Renderer::StaticShutdown()
{
    OpenGLAPI::ClearSymbols();
    SDL_GL_UnloadLibrary();
}

static void __stdcall debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    if(severity != GL_DEBUG_SEVERITY_NOTIFICATION)
        logerror("GL[%u]: %s\n", severity, message);
}

// --------------------------------------------------------------------------------------

Renderer::Renderer(SDL_Window *win)
: window(win)
, glctx(NULL)
{
}

Renderer::~Renderer()
{
    SDL_GL_DeleteContext(glctx);
}

bool Renderer::init()
{
    glctx = SDL_GL_CreateContext(window);
    if(!glctx)
        return false;

    if(!s_symbolsLoaded)
    {
        s_symbolsLoaded = true;
        if(!OpenGLAPI::LoadSymbols())
        {
            SDL_GL_DeleteContext(glctx);
            return false;
        }
    }

    SDL_GL_MakeCurrent(window, glctx);

    // enable vsync to burn less CPU
    if(SDL_GL_SetSwapInterval(-1) == -1)
        SDL_GL_SetSwapInterval(1);

#ifdef _DEBUG
    if(glDebugMessageCallback)
    {
        puts("Using GL debug callbacks");
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debugCallback, NULL);
    }
    else
        puts("glDebugMessageCallback() not supported");
#endif

    glClearColor(0, 0, 0, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    return true;
}

unsigned int Renderer::getFreeVideoMemoryKB()
{
    GLint meminfo[4];
    memset(meminfo, 0, 4 * sizeof(GLint));

    // nvidia?
    glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, meminfo); // in KB
    if(!meminfo[0])
    {
        // ATI?
        glGetIntegerv(TEXTURE_FREE_MEMORY_ATI, meminfo); // in KB, // [0] = total memory free in the pool
    }
    GLenum err = glGetError();

    return meminfo[0];
}

void Renderer::beginFrame()
{
    OpenGLAPI::ResetCallCount();
    SDL_GL_MakeCurrent(window, glctx);
}

void Renderer::endFrame()
{
}

unsigned int Renderer::getRenderCallCount()
{
    return OpenGLAPI::GetCallCount();
}

void Renderer::clear()
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::show()
{
    SDL_GL_SwapWindow(window);
}


// -----------------------------------------------------------------------------------

void Renderer::destroyTex(unsigned id)
{
    glDeleteTextures(1, &id);
}

unsigned Renderer::loadTex(const Image *img)
{
    GLint pack, unpack;
    glGetIntegerv(GL_PACK_ALIGNMENT, &pack);
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLuint texId = 0;
    glGenTextures(1, &texId);
    if(!texId)
        return 0;

    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // No mipmapping for now.
    // TODO: Maybe later?
    glTexImage2D(GL_TEXTURE_2D, 0, 4, (GLsizei)img->w(), (GLsizei)img->h(), 0, GL_RGBA, GL_FLOAT, img->data());

    glPixelStorei(GL_PACK_ALIGNMENT, pack);
    glPixelStorei(GL_UNPACK_ALIGNMENT, unpack);

    return texId;
}


