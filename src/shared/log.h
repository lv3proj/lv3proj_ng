#ifndef _LOG_H
#define _LOG_H

#include "common.h"

#if __clang__ || __GNUC__
#  define ATTR_NORETURN __attribute__((noreturn))
#  define ATTR_PRINTF(F,V) __attribute__ ((format (printf, F, V)))
#else //COMPILER != COMPILER_GNU
#  define ATTR_NORETURN
#  define ATTR_PRINTF(F,V)
#endif //COMPILER == COMPILER_GNU

enum ConsoleColor
{
    BLACK,
    RED,
    GREEN,
    BROWN,
    BLUE,
    MAGENTA,
    CYAN,
    GREY,
    YELLOW,
    LRED,
    LGREEN,
    LBLUE,
    LMAGENTA,
    LCYAN,
    WHITE
};

// text, color, level, userdata
typedef void (*log_callback_func)(const char *, int, int, void *);

void log_prepare(const char *fn, const char *mode);
void log_setcallback(log_callback_func f, bool newline, void *user);
void log_setloglevel(int lvl);
void log_setlogtime(bool b);
void log(const char *str, ...) ATTR_PRINTF(1,2);
void logdetail(const char *str, ...) ATTR_PRINTF(1,2);
void logdebug(const char *str, ...) ATTR_PRINTF(1,2);
void logdev(const char *str, ...) ATTR_PRINTF(1,2);
void logerror(const char *str, ...) ATTR_PRINTF(1,2);
void logcritical(const char *str, ...) ATTR_PRINTF(1,2);
void logcustom(int loglevel, ConsoleColor color, const char *str, ...) ATTR_PRINTF(3,4);
void log_close();
void _log_setcolor(bool, ConsoleColor);
void _log_resetcolor(bool);


const int Color_count = int(WHITE)+1;

#ifdef _DEBUG
#  define DEBUG_LOG logdebug
#else
#  define DEBUG_LOG
#endif

#endif
