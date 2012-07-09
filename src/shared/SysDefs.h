#ifndef _SYSDEFS_H
#define _SYSDEFS_H

//////////////////////////////////////
// Platform defines
//////////////////////////////////////

#define PLATFORM_WIN32 0
#define PLATFORM_UNIX  1
#define PLATFORM_APPLE 2

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
#  define PLATFORM PLATFORM_WIN32
#elif defined( __APPLE_CC__ )
#  define PLATFORM PLATFORM_APPLE
#else
#  define PLATFORM PLATFORM_UNIX
#endif

#define COMPILER_MICROSOFT 0
#define COMPILER_GNU       1
#define COMPILER_BORLAND   2
#define COMPILER_INTEL     3

#ifdef _MSC_VER
#  define COMPILER COMPILER_MICROSOFT
#elif defined( __BORLANDC__ )
#  define COMPILER COMPILER_BORLAND
#elif defined( __INTEL_COMPILER )
#  define COMPILER COMPILER_INTEL
#elif defined( __GNUC__ )
#  define COMPILER COMPILER_GNU
#else
#  pragma warning "WARNING: Unknown compiler."
#endif

#if COMPILER == COMPILER_MICROSOFT
#  pragma warning( disable : 4267 )    // conversion from 'size_t' to 'int', possible loss of data
#  pragma warning( disable : 4786 )    // identifier was truncated to '255' characters in the debug information
#  pragma warning( disable : 4800 )    // conversion to bool, performance warning
#  pragma warning( disable : 4244 )    // conversion from 'uint64' to 'int16', possible loss of data
#  pragma warning( disable : 4996 )    // disable warning for "too old" functions (VC80)
#  pragma warning( disable : 4355 )    // 'this' : used in base member initializer list
#endif

////////////////////////////////////
// Compiler defines
////////////////////////////////////

#if COMPILER == COMPILER_MICROSOFT
    #define I64FMT "%016I64X"
    #define I64FMTD "%I64u"
    #define I64LIT(x) (x ## i64)
    #define UI64LIT(x) (x ## ui64)
    #define snprintf _snprintf
    // TODO: fix for win64, if required
    typedef __int64            int64;
    typedef long               int32;
    typedef short              int16;
    typedef char               int8;
    typedef unsigned __int64   uint64;
    typedef unsigned long      uint32;
    typedef unsigned short     uint16;
    typedef unsigned char      uint8;
#else
    #include <stdint.h>
    #define stricmp strcasecmp
    #define strnicmp strncasecmp
    #define I64FMT "%016llX"
    #define I64FMTD "%llu"
    #define I64LIT(x) (x ## LL)
    #define UI64LIT(x) (x ## ULL)
    typedef int64_t int64;
    typedef int32_t int32;
    typedef int16_t int16;
    typedef char int8;
    typedef uint64_t uint64;
    typedef uint32_t uint32;
    typedef uint16_t uint16;
    typedef unsigned char uint8;
#endif

#ifndef _LP64
#   if defined (_M_IA64) || defined (__ia64__) || defined (_M_AMD64) || defined (__amd64) || defined(_M_X64)
#      define _LP64 1
#   endif
#endif

#ifdef _LP64 // to be set for 64 bit compile
#   define PTRFMT "0x"I64FMT
#   define SYSTEM_BITS 64
#else
#   define PTRFMT "0x%X"
#   define SYSTEM_BITS 32
#endif

#ifndef SIGQUIT
#define SIGQUIT 3
#endif

#if COMPILER == COMPILER_MICROSOFT
#  if _MSC_VER >= 1600
#     define COMPILER_NAME "VC100+"
#  elif _MSC_VER >= 1500
#    define COMPILER_NAME "VC90"
#  elif _MSC_VER >= 1400
#    define COMPILER_NAME "VC80"
#  elif _MSC_VER >= 1310
#    define COMPILER_NAME "VC71"
#  endif
#  define COMPILER_VERSION _MSC_VER
#  define COMPILER_VERSION_OUT "%u"
#elif COMPILER == COMPILER_GNU
#  define COMPILER_NAME "GCC"
#  ifndef __GNUC_PATCHLEVEL__
#    define __GNUC_PATCHLEVEL__ 0
#  endif
#  define COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#  define COMPILER_VERSION_OUT "%u"
// TODO: add more compilers here when necessary
#else
#  define COMPILER_NAME "unknown"
#  define COMPILER_VERSION "unk"
#  define COMPILER_VERSION_OUT "%s"
#endif

#if PLATFORM == PLATFORM_UNIX
# define PLATFORM_NAME "Unix"
#elif PLATFORM == PLATFORM_WIN32
# define PLATFORM_NAME "Win32"
#elif PLATFORM == PLATFORM_APPLE
# define PLATFORM_NAME "Apple"
// TODO: add more platforms here when necessary
#else
# define PLATFORM_NAME "unknown"
#endif

#if COMPILER == COMPILER_GNU
#  define ATTR_NORETURN __attribute__((noreturn))
#  define ATTR_PRINTF(F,V) __attribute__ ((format (printf, F, V)))
#else //COMPILER != COMPILER_GNU
#  define ATTR_NORETURN
#  define ATTR_PRINTF(F,V)
#endif //COMPILER == COMPILER_GNU


// taken from ACE
// have seen on some systems that both defines exist, so if that is is the case, rely on this detection here
#if (!defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN)) || (defined(LITTLE_ENDIAN) && defined(BIG_ENDIAN))
#  if defined (i386) || defined (__i386__) || defined (_M_IX86) || \
      defined (vax) || defined (__alpha) || defined (__LITTLE_ENDIAN__) || \
      defined (ARM) || defined (_M_IA64) || defined (__ia64__) || \
      defined (_M_AMD64) || defined (__amd64)
         // We know these are little endian.
#        define IS_LITTLE_ENDIAN 1
#        undef IS_BIG_ENDIAN
#  else
         // Otherwise, we assume big endian.
#        undef IS_LITTLE_ENDIAN
#        define IS_BIG_ENDIAN 1
#  endif
#endif


#ifndef SIGQUIT
#define SIGQUIT 3
#endif


#endif
