#ifndef BITHACKS_H
#define BITHACKS_H

#include "SysDefs.h"

#ifdef _MSC_VER
#  include <intrin.h>
#endif

namespace bithacks {

// floor to next power of 2
FORCE_INLINE uint32 flp2(uint32 x)
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return x - (x >> 1);
}

// ceil to next power of 2
FORCE_INLINE uint32 clp2(uint32 x)
{
	--x;
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return x + 1;
}

FORCE_INLINE uint32 popcnt(uint32 x)
{
	x -= ((x >> 1) & 0x55555555);
	x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
	x = (((x >> 4) + x) & 0x0f0f0f0f);
	x += (x >> 8);
	x += (x >> 16);
	return x & 0x0000003f;
}

FORCE_INLINE uint32 ctz(uint32 x)
{
#ifdef __GNUC__
	return __builtin_ctz(x);
#elif _MSC_VER && defined(_M_IX86)
	unsigned long r = 0;
	_BitScanReverse(&r, x);
	return r;
#else
	return popcnt((x & -x) - 1);
#endif
}

// FIXME: verify this
FORCE_INLINE unsigned int clz(uint32 x)
{
#ifdef __GNUC__
	return __builtin_ctz((value));
#elif defined(_MSC_VER) && defined(_M_IX86)
	unsigned long r = 0;
	_BitScanForward(&r, x);
	return r;
#else
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return 32 - popcnt(x);
#endif
}






}; // end namespace bithacks

#endif
