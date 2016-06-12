#ifndef MATHTOOLS_H
#define MATHTOOLS_H

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


#endif
