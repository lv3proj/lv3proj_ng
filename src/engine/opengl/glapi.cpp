#include "SDLMaster.h"

#include "glapi.h"

#include <stdio.h>


// Populate global namespace with static function pointers pFUNC,
// and function stubs FUNC that call their associated function pointer
#define GL_FUNC(ret,fn,params,call,rt) \
    extern "C" { \
    static ret (GLAPIENTRY *p##fn) params = NULL; \
    ret GLAPIENTRY fn params { rt p##fn call; } \
    }

#define GL_PTR(pty, fn) pty fn = NULL;
#include "glstubs.h"

PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback; // HACK: optional

template<typename T> bool getsym(const char *sym, T **ptr)
{
    if(!(*ptr = (T*)SDL_GL_GetProcAddress(sym)))
    {
        printf("Failed to get GL sym: %s\n", sym);
        return false;
    }
    //printf("%s -> %p\n", sym, *ptr);
    return true;
}


static bool lookup_all_glsyms(void)
{
    getsym("glDebugMessageCallback", &glDebugMessageCallback); // HACK: optional

    return
#define GL_PTR(_, fn) getsym(#fn, &fn) &
#define GL_FUNC(ret,fn,params,call,rt) getsym(#fn, &p##fn) &
#include "glstubs.h"
        true;
}



namespace OpenGLAPI {


bool LoadSymbols()
{
    bool ok = lookup_all_glsyms();
    const char *ver = (const char*)glGetString(GL_VERSION);
    printf("GL version: %s\n", ver);
    const char *ven = (const char*)glGetString(GL_VENDOR);
    printf("GL vendor: %s\n", ven);
    //const char *ext = (const char*)glGetString(GL_EXTENSIONS);
    //printf("GL ext: %s\n", ext);
    return ok;
}

void ClearSymbols()
{
    #define GL_FUNC(ret,fn,params,call,rt) p##fn = NULL;
    #define GL_PTR(pty, fn) fn = NULL;
    #include "glstubs.h"
}

void ResetCallCount()
{
    //s_glCalls = 0;
}

unsigned GetCallCount()
{
    //return s_glCalls;
    return 0;
}


}; // end namespace OpenGLAPI

