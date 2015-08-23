#ifndef _COMMON_H
#define _COMMON_H

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

#ifdef _DEBUG
#  include <typeinfo>
#  define ASSERT_DYNAMIC_TYPE(what, type) ASSERT(typeid(what) == typeid(type))
#else
#  define ASSERT_DYNAMIC_TYPE(what, type) /* no RTTI in release mode */
#endif

#include <stdio.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <string>
#include <sstream>
#include <list>
#include <deque>
#include <vector>

#include "SysDefs.h"
#include "PlatformSpecific.h"


struct memblock
{
    memblock() : ptr(NULL), size(0) {}
    memblock(uint8 *p, uint32 s) : size(s), ptr(p) {}
    uint8 *ptr;
    uint32 size;
};


#include "log.h"
#include "tools.h"


#endif

