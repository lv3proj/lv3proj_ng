#ifndef MATHTOOLS_H
#define MATHTOOLS_H

#include <algorithm>

#include "SysDefs.h"

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

const float PI_F = float(M_PI);

const float DEGTORAD = PI_F / 180.0f;
const float RADTODEG = 180.0f / PI_F;
const float PI_HALF  = PI_F / 2.0f;


inline float radToDeg(float rad)
{
    return RADTODEG * rad;
}

inline float degToRad(float deg)
{
    return DEGTORAD * deg;
}

// returns linear interpolation of a and b with ratio t, with 0 <= t <= 1
template<class T> inline T lerp(const T& a, const T& b, const T& t)
{
    return (a * (1-t)) + (b*t);
}

// clamps a value between low and high
template <class T> inline const T& clamp (const T& value, const T& low, const T& high)
{
    return std::min<T>(std::max<T>(value,low), high);
}

template <class T> inline bool exceeds(const T& v, const T& mx)
{
    return (!mx && v) || (mx < 0 ? v < mx : v > mx);
}

template <class T> inline const T sgn(const T val)
{
    if(val > 0)
        return 1;
    else if(val < 0)
        return -1;
    return 0;
}

//  2.1 ->  2.0 (floor)
//  2.8 ->  2.0 (floor)
// -2.1 -> -2.0 (ceil)
// -2.8 -> -2.0 (ceil)
inline int ftrunci(float val)
{
    if(val > 0)
        return floor(val);
    else if(val < 0)
        return ceil(val);
    return 0.0f;
}

//  2.1 ->  3.0 (ceil)
//  2.8 ->  3.0 (ceil)
// -2.1 -> -3.0 (floor)
// -2.8 -> -3.0 (floor)
inline int fceili(float val)
{
    if(val > 0)
        return ceil(val);
    else if(val < 0)
        return floor(val);
    return 0.0f;
}

// scale t from [lower, upper] into [rangeMin, rangeMax]
// Allows out of bounds values
template <typename T> inline T rangeTransform(T t, T lower, T upper, T rangeMin, T rangeMax)
{
    T d = upper - lower;
    if (d == 0)
        return rangeMin;

    t -= lower;
    t /= d;
    t *= (rangeMax - rangeMin);
    t += rangeMin;
    return t;
}

template <typename T> inline T sq(T a)
{
    return a * a;
}



#endif
