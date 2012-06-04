#ifndef _COMMON_H
#define _COMMON_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <string>
#include <sstream>
#include <list>
#include <deque>
#include <vector>

#include "SysDefs.h"
#include "log.h"
#include "tools.h"

struct memblock
{
    memblock() : ptr(NULL), size(0) {}
    memblock(uint8 *p, uint32 s) : size(s), ptr(p) {}
    uint8 *ptr;
    uint32 size;
};

#define compile_assert(pred) {switch(0) { case 0: case (pred): ;}}

#ifndef NDEBUG
#  define DBG if(1)
#else
#  define DBG if(0)
#endif


#endif

