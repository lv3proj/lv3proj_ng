#include "Bootstrap.h"
#include "common.h"
#include "Engine.h"
#include <signal.h>
#include "tools.h"
#include "PlatformSpecific.h"

namespace Bootstrap {


// never executed, but the compile_assert() macro has to be in a function, so there...
static void _nop_compile_checks()
{
    compile_assert(sizeof(int8) == 1);
    compile_assert(sizeof(int16) == 2);
    compile_assert(sizeof(int32) == 4);
    compile_assert(sizeof(int64) == 8);

    compile_assert(sizeof(uint8) == 1);
    compile_assert(sizeof(uint16) == 2);
    compile_assert(sizeof(uint32) == 4);
    compile_assert(sizeof(uint64) == 8);
}

static void _OnSignal(int s)
{
    switch(s)
    {
    case SIGINT:
    case SIGQUIT:
    case SIGTERM:
    case SIGABRT:
#ifdef _WIN32
    case SIGBREAK:
#endif
        engine->SetQuit(true);
        break;
    }
}

void HookSignals()
{
    signal(SIGINT, &_OnSignal);
    signal(SIGQUIT, &_OnSignal);
    signal(SIGTERM, &_OnSignal);
    signal(SIGABRT, &_OnSignal);
#ifdef _WIN32
    signal(SIGBREAK, &_OnSignal);
#endif
}

void UnhookSignals()
{
    signal(SIGINT, 0);
    signal(SIGQUIT, 0);
    signal(SIGTERM, 0);
    signal(SIGABRT, 0);
#ifdef _WIN32
    signal(SIGBREAK, 0);
#endif
}

void PrintSystemSpecs(void)
{
    logcustom(0, LGREEN, "System/EngineBase specs:");
    logcustom(0, LGREEN, "----------------------------------");
    logcustom(0, LGREEN, "Platform: %s", PLATFORM_NAME);
    logcustom(0, LGREEN, "Compiler: %s (" COMPILER_VERSION_OUT ")", COMPILER_NAME, COMPILER_VERSION);
    logcustom(0, LGREEN, "Endian:   %s", IS_LITTLE_ENDIAN+0 ? "little" : "big");
    logcustom(0, LGREEN, "Bits:     %u", SYSTEM_BITS);
    logcustom(0, LGREEN, "----------------------------------");
}

bool RelocateWorkingDir(void)
{
    std::string procdir = GetProgramDir();
    if(procdir.length())
    {
        if(SetWorkingDir(procdir))
        {
            logdetail("Working directory successfully changed to program directory:");
            logdetail(" '%s'", procdir.c_str());
            return true;
        }
        else
        {
            logerror("Unable to change working directory to program directory:");
            logerror(" '%s'", procdir.c_str());
        }
    }
    else
    {
        logerror("WARNING: Unable to detect program directory! Be sure to run this from the correct path or set the working dir manually, "
            "otherwise the engine may not find its data and will be unable to start up!");
    }
    return false;
}



} // end namespace Bootstrap
