#ifndef BITHACKS_H
#define BITHACKS_H

#ifdef _MSC_VER
#  include <intrin.h>
#endif

namespace bithacks {

// floor to next power of 2
inline unsigned flp2(unsigned x)
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return x - (x >> 1);
}

// ceil to next power of 2
inline unsigned clp2(unsigned x)
{
	--x;
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return x + 1;
}

inline bool isPowerOf2(unsigned x)
{
    return x && !(x & (x - 1));
}

inline unsigned popcnt(unsigned x)
{
	x -= ((x >> 1) & 0x55555555);
	x = (((x >> 2) & 0x33333333) + (x & 0x33333333));
	x = (((x >> 4) + x) & 0x0f0f0f0f);
	x += (x >> 8);
	x += (x >> 16);
	return x & 0x0000003f;
}

inline unsigned ctz(unsigned x)
{
#ifdef __GNUC__
	return __builtin_ctz(x);
#elif defined(_MSC_VER) && defined(_M_IX86)
	unsigned long r = 0;
	_BitScanForward(&r, x);
	return r;
#else
	return popcnt((x & -x) - 1);
#endif
}

// FIXME: verify this
inline unsigned int clz(unsigned x)
{
#ifdef __GNUC__
	return __builtin_clz(x);
#elif defined(_MSC_VER) && defined(_M_IX86)
	unsigned long r = 0;
	_BitScanReverse(&r, x);
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
