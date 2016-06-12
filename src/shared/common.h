#ifndef _COMMON_H
#define _COMMON_H

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#include <stddef.h>
#include <stdlib.h>

#define compile_assert(pred) {switch(0) { case 0: case (pred): ;}}

#ifndef NDEBUG
#  define DBG if(1)
#else
#  define DBG if(0)
#endif

#ifdef NDEBUG
#  define ASSERT(x)
#else
#  define ASSERT(x) { if(!(x)) { fprintf(stderr, "ASSERTION FAILED: \"%s\", File: %s:%u\n", #x, __FILE__, __LINE__); TriggerBreakpoint(); } }
#endif

#include <stdio.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <string>

#include "PlatformSpecific.h"


/*
struct memblock
{
    memblock() : ptr(NULL), size(0) {}
    memblock(unsigned char *p, unsigned s) : size(s), ptr(p) {}
    unsigned char *ptr;
    unsigned size;
};
*/

/*
struct Rect
{
    unsigned x, y, w, h;

    inline bool isZero() const { return !(x | y | w | h); }
};
*/

#include "log.h"
#include "tools.h"


#endif

