#include "PlatformSpecific.h"

#if _WIN32
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   include <direct.h>
#else
#   if PLATFORM == PLATFORM_APPLE
#        include <Carbon/Carbon.h>
#        include <sys/param.h>
#   endif
#   include <sys/stat.h>
#   include <sys/timeb.h>
#   include <sys/types.h>
#   include <sys/ioctl.h>
#   include <unistd.h>
#endif

#include <signal.h>
#include "log.h"


// fix filenames for linux ( '/' instead of windows '\')
static void _FixFileName(std::string& str)
{
    for(size_t i = 0; i < str.length(); i++)
        if(str[i]=='\\')
            str[i]='/';
}


// taken from a post from http://www.gamedev.net/topic/459511-something-like-getmodulefilename-in-linux/
std::string GetProgramDir(void)
{
#ifdef _WIN32
    char szPath[1024];
    size_t len = GetModuleFileName( NULL, szPath, 1024 );
    if(!len)
        return "";
    std::string path(szPath);
    std::string::size_type t = path.find_last_of("/\\");
    path = path.substr(0,t);
    _FixFileName(path);
    return path;

#elif defined(__APPLE_CC__)

    char parentdir[MAXPATHLEN];
    std::string path;
    CFURLRef url = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    CFURLRef url2 = CFURLCreateCopyDeletingLastPathComponent(0, url);
    if (CFURLGetFileSystemRepresentation(url2, 1, (unsigned char*)parentdir, MAXPATHLEN))
    {
        path = parentdir;   /* chdir to the binary app's parent */
    }
    CFRelease(url);
    CFRelease(url2);
    return path;

#else
    std::stringstream ss;
    ss << "/proc/" << getpid() << "/exe";
    char proc[1024];
    int ch = readlink(ss.str().c_str(), proc, 1024);
    std::string path;
    if (ch != -1)
    {
        proc[ch] = 0;
        path = proc;
        std::string::size_type t = path.find_last_of("/");
        path = path.substr(0,t);
    }
    return path;

#endif
}

bool SetWorkingDir(const char *d)
{
#if _WIN32
    return !_chdir(d);
#else
    return !chdir(d);
#endif
}

std::string GetWorkingDir(void)
{
    char *wd;
#if PLATFORM == PLATFORM_WIN32
    wd = _getcwd(NULL, 0);
#elif PLATFORM == PLATFORM_UNIX || PLATFORM == PLATFORM_APPLE
    wd = getcwd(NULL, 0);
#endif
    std::string cp = wd;
    free(wd);
    return cp;
}


unsigned GetConsoleWidth(void)
{
#if PLATFORM == PLATFORM_WIN32
    HANDLE hOut;
    CONSOLE_SCREEN_BUFFER_INFO SBInfo;
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hOut,
        &SBInfo);
    return SBInfo.dwSize.X;
#else
    struct winsize ws;
    if (ioctl(0,TIOCGWINSZ,&ws))
        return 80; // the standard, because we don't know any better
    return ws.ws_col;
#endif
}

void TriggerBreakpoint()
{
#ifdef _MSC_VER
    _CrtDbgBreak();
#elif defined(__GNUC__) && ((__i386__) || (__x86_64__))
    __asm__ __volatile__ ( "int $3\n\t" );
#else
    raise(SIGTRAP);
#endif
}

// FIXME: move this to engine and use SDL simple message box if not on windows
void MsgBox(const char *s)
{
    logerror("%s", s);
#ifdef _WIN32
    MessageBoxA(NULL, s, "Message", MB_OK);
#endif
}
