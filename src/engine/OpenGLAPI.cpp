
#include <SDL/SDL.h>

#include "GL/gl.h"


#include "OpenGLAPI.h"
#include "log.h"

#ifdef _WIN32
#define GLAPIENTRY __stdcall
#else
#define GLAPIENTRY
#endif

// Populate global namespace with static function pointers pFUNC,
// and function stubs FUNC that call their associated function pointer
#define GL_FUNC(ret,fn,params,call,rt) \
    extern "C" { \
    static ret (GLAPIENTRY *p##fn) params = NULL; \
    ret fn params { rt p##fn call; } \
    }
#include "OpenGLStubs.h"
#undef GL_FUNC

static bool lookup_glsym(const char *funcname, void **func)
{
    *func = SDL_GL_GetProcAddress(funcname);
    if (*func == NULL)
    {
        logcritical("Failed to find OpenGL symbol '%s'", funcname);
        return false;
    }
    logdev("%s -> " PTRFMT, funcname, *func);
    return true;
}

static bool lookup_all_glsyms(void)
{
    bool retval = true;
#define GL_FUNC(ret,fn,params,call,rt) \
    if (!lookup_glsym(#fn, (void **) &p##fn)) retval = false;
#include "OpenGLStubs.h"
#undef GL_FUNC
    return retval;
}



namespace OpenGLAPI {


bool LoadSymbols()
{
    return lookup_all_glsyms();
}

void ClearSymbols()
{
    // reset all the entry points to NULL, so we know exactly what happened
    //  if we call a GL function after shutdown.
    #define GL_FUNC(ret,fn,params,call,rt) p##fn = NULL;
    #include "OpenGLStubs.h"
    #undef GL_FUNC
}


}; // end namespace OpenGLAPI

