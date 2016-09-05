#include "SDLMaster.h"
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "renderer.h"
#include "glapi.h"
#include "io/image.h"
#include "glmx.h"
#include "Objects.h"

using namespace glm;

bool Renderer::s_symbolsLoaded = false;

Renderer *g_renderer = NULL;


bool Renderer::StaticInit()
{
    s_symbolsLoaded = false;

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
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
    g_renderer = this;
}

Renderer::~Renderer()
{
    g_renderer = NULL;
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

void Renderer::beginFrame()
{
    OpenGLAPI::ResetCallCount();
    SDL_GL_MakeCurrent(window, glctx);
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    glViewport(0, 0, w, h);
}

void Renderer::endFrame()
{
}

void Renderer::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::show()
{
    SDL_GL_SwapWindow(window);
}

static const vec2 quadVertices[] =
{
    vec2(-1, -1), // upper left
    vec2(+1, -1), // upper right
    vec2(-1, +1), // lower left
    vec2(+1, +1), // lower right
};

unsigned Renderer::renderObj(const mat4 &proj, const BaseObject *obj)
{
    switch(obj->getType())
    {
    case RO_SPRITE:
        return renderSprite(proj, (Sprite*)obj);
    case RO_GROUP:
        return renderGroup(proj, (GroupObject*)obj);
    default:
        ASSERT(false);
    }
    return 0;
}

unsigned Renderer::renderGroup(mat4 proj, const GroupObject *obj)
{
    const size_t sz = obj->size();
    if(sz)
    {
        proj *= obj->getLocalTransform();
        for(size_t i = 0; i < sz; ++i)
            renderObj(proj, obj->child(i));
    }
    return unsigned(sz);
}

unsigned Renderer::renderSprite(mat4 proj, const Sprite *obj)
{
    obj->tex->bind();
    mat4 mat = proj * obj->getLocalTransform();
    glLoadMatrixf(value_ptr(mat));
    glColor4fv(value_ptr(obj->color));
    glVertexPointer(2, GL_FLOAT, 0, value_ptr(quadVertices[0]));
    glTexCoordPointer(2, GL_FLOAT, 0, value_ptr(obj->uv[0]));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    return 1;
}

