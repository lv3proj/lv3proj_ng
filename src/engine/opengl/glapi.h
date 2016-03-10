#ifndef OPENGL_API_H
#define OPENGL_API_H

/*#define GL_GLEXT_PROTOTYPES
#include "GL/gl.h"
#include "GL/glext.h"*/


#ifdef _DLL
#define WASDLL
#undef _DLL // HACK: don't put any __declspec in front of the function definitions
#endif

//#define APIENTRY
#include "SDL2/SDL_opengl.h"
#include "SDL2/SDL_opengl_glext.h"

// Generate declarations
#define GL_FUNC(ret,fn,params,call,rt)
#define GL_PTR(pty, fn) extern pty fn;
#include "glstubs.h"

#ifdef WASDLL
#define _DLL
#endif

// HACK: optional
extern PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;

namespace OpenGLAPI
{
    bool LoadSymbols();
    void ClearSymbols();
    void ResetCallCount();
    unsigned GetCallCount();
};


#endif
