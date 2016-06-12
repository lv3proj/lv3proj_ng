/*
Copyright (C) 2007, 2010 - Bit-Blot

This file is part of Aquaria.

Aquaria is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifndef VECTOR_H
#define VECTOR_H


#include "common.h"
#include "mathtools.h"
#include "MemorySystem.h"

typedef float scalar_t;

//#define _f_equals(a, b) (abs((a) - (b)) < 0.000001f)

inline static bool _f_equals(const scalar_t a, const scalar_t b)
{
    return abs(a - b) < 0.000001f;
}

class Vector
{
protected:

public:
    scalar_t x;
    scalar_t y;
    scalar_t z;    // x,y,z coordinates

    Vector(scalar_t a = 0, scalar_t b = 0, scalar_t c = 0) : x(a), y(b), z(c) {}
    Vector(const Vector &vec) : x(vec.x), y(vec.y), z(vec.z) {}

    static inline Vector FromRadAngle2D(float rad)
    {
        return Vector(sin(rad), cos(rad));
    }

    static inline Vector FromDegAngle2D(float deg)
    {
        return FromRadAngle2D(degToRad(deg));
    }

    const Vector &operator=(const Vector &vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
        return *this;
    }

    const Vector &operator=(const scalar_t &s)
    {
        x = s;
        y = 0;
        z = 0;
        return *this;
    }

    // vector equality
    const bool operator==(const Vector &vec) const
    {
        return (_f_equals(x, vec.x) && _f_equals(y, vec.y) && _f_equals(z, vec.z));
    }

    // vecector inequality
    const bool operator!=(const Vector &vec) const
    {
        return !(*this == vec);
    }

    // vector add
    const Vector operator+(const Vector &vec) const
    {
        return Vector(x + vec.x, y + vec.y, z + vec.z);
    }

    // vector add (opposite of negation)
    const Vector operator+() const
    {    
        return Vector(*this);
    }

    // vector increment
    const Vector& operator+=(const Vector& vec)
    {    x += vec.x;
    y += vec.y;
    z += vec.z;
    return *this;
    }

    // vector subtraction
    const Vector operator-(const Vector& vec) const
    {    
        return Vector(x - vec.x, y - vec.y, z - vec.z);
    }

    // vector negation
    const Vector operator-() const
    {    
        return Vector(-x, -y, -z);
    }

    bool isZero() const
    {
        return x == 0 && y == 0 && z == 0;
    }

    // vector decrement
    const Vector &operator-=(const Vector& vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;

        return *this;
    }

    // scalar self-multiply
    const Vector &operator*=(const scalar_t &s)
    {
        x *= s;
        y *= s;
        z *= s;

        return *this;
    }

    // scalar self-divecide
    const Vector &operator/=(const scalar_t &s)
    {
        const scalar_t recip = 1/s;

        x *= recip;
        y *= recip;
        z *= recip;

        return *this;
    }

    // vector self-divide
    const Vector &operator/=(const Vector &v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;

        return *this;
    }

    const Vector &operator*=(const Vector &v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;

        return *this;
    }


    // post multiply by scalar
    const Vector operator*(const scalar_t &s) const
    {
        return Vector(x*s, y*s, z*s);
    }

    // post multiply by Vector
    const Vector operator*(const Vector &v) const
    {
        return Vector(x*v.x, y*v.y, z*v.z);
    }

    // pre multiply by scalar
    friend inline const Vector operator*(const scalar_t &s, const Vector &vec)
    {
        return vec*s;
    }

    // divide by scalar
    const Vector operator/(scalar_t s) const
    {
        s = 1/s;

        return Vector(s*x, s*y, s*z);
    }

    Vector inline getPerpendicularLeft() const
    {
        return Vector(-y, x);
    }

    Vector inline getPerpendicularRight() const
    {
        return Vector(y, -x);
    }

    // cross product
    const Vector crossProd(const Vector &vec) const
    {
        return Vector(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
    }

    // dot product
    const scalar_t inline dot(const Vector &vec) const
    {
        return x*vec.x + y*vec.y + z*vec.z;
    }

    const scalar_t inline dot2D(const Vector &vec) const
    {
        return x*vec.x + y*vec.y;
    }

    // length of vector
    const scalar_t inline getLength3D() const
    {
        return sqrt(x*x + y*y + z*z);
    }
    const scalar_t inline getLength2D() const
    {
        return sqrt(x*x + y*y);
    }

    // length of vector, squared
    const scalar_t inline getLength3DSq() const
    {
        return x*x + y*y + z*z;
    }
    const scalar_t inline getLength2DSq() const
    {
        return x*x + y*y;
    }

    // return the unit vector
    const Vector inline getUnit3D() const
    {
        return (*this) * (1/getLength3D());
    }

    // normalize this vector
    void inline normalize3D()
    {
        if(!isZero())
        {
            (*this) *= 1/getLength3D();
        }
    }
    void inline normalize2D()
    {
        if(!(x == 0 && y == 0))
        {
            (*this) *= 1/getLength2D();
        }
    }

    void inline setLength3D(const float l)
    {
        if (l == 0)
        {
            x = y = z = 0;
        }
        else
        {
            scalar_t fact = l / getLength3D();
            this->x *= fact;
            this->y *= fact;
            this->z *= fact;
        }
    }
    void inline setLength2D(const float l)
    {
        if (!isZero())
        {
            scalar_t fact = l / getLength2D();
            this->x *= fact;
            this->y *= fact;
        }
    }

    inline bool operator!() const
    {
        return isZero();
    }

    const bool inline isLength2DIn(scalar_t radius) const
    {
        return (x*x + y*y) <= (radius*radius);
    }

    // return angle between two vectors -- not passed by reference intentionally
    const scalar_t inline angleRad2D(Vector v) const
    {
        Vector m = *this;
        v.normalize2D();
        m.normalize2D();
        return m._angleNorm2D(v);
    }

    const scalar_t inline angle2D(const Vector& v) const
    {
        return radToDeg(angleRad2D(v));
    }

    // return angle between two vectors -- both vectors must already be normalized!
    const scalar_t inline _angleNorm2D(const Vector& normal) const
    {
        return acos(dot2D(normal));
    }

    // reflect this vector off surface with normal vector
    // FIXME: check that this is correct
    const Vector inline getReflection2D(const Vector& normal) const
    {
        if (!isZero())
        {
            const scalar_t len = getLength2D();
            const Vector unit = *this / len;
            Vector ret = 2*(-unit.dot(normal))*normal + unit;
            ret.setLength2D(len);
            //ret *= len;
            return ret;
        }
        return Vector(0, 0);
    }

    const void inline setZero()
    {
        this->x = this->y = this->z = 0;
    }

    const bool inline isNan() const
    {
#ifdef _WIN32
        return _isnan(x) || _isnan(y) || _isnan(z);
#else
        return isnan(x) || isnan(y) || isnan(z);
#endif
    }

    // rotate in degrees
    inline void rotate2D(float angle)
    {
        rotateRad2D(degToRad(angle));
    }

    inline void rotateRad2D(float rad)
    {
        scalar_t oldx = x;
        x = cos(rad)*x - sin(rad)*y;
        y = sin(rad)*oldx + cos(rad)*y;
    }

    const inline scalar_t rotationRad2D() const
    {
        if(x == 0 && y == 0)
            return 0;
        return atan2(y, x) + (DEGTORAD * 90); // FIXME: verify this
    }

    const inline scalar_t rotation2D() const
    {
        if(x == 0 && y == 0)
            return 0;

        return radToDeg(atan2(y, x)) + 90;
    }

    inline Vector project2DOnto(const Vector& b) const
    {
        float tmp = dot(b) / b.getLength2DSq();
        return Vector(tmp * b.x, tmp * b.y);
        /*Vector proj;
        const Vector& a = *this;
        float dp = (a.x*b.x + a.y*b.y);
        proj.x = ( dp / (b.x*b.x + b.y*b.y) ) * b.x;
        proj.y = ( dp / (b.x*b.x + b.y*b.y) ) * b.y;
        return proj;*/
    }

};

/*
class VectorPathNode
{
public:
    VectorPathNode() { percent = 0; }

    Vector value;
    float percent;
};

class VectorPath
{
public:
    void flip();
    void clear();
    void addPathNode(Vector v, float p);
    Vector getValue(float percent);
    int getNumPathNodes() { return pathNodes.size(); }
    void resizePathNodes(int sz) { pathNodes.resize(sz); }
    VectorPathNode *getPathNode(int i) { if (i<getNumPathNodes() && i >= 0) return &pathNodes[i]; return 0; }
    void cut(int n);
    void splice(const VectorPath &path, int sz);
    void prepend(const VectorPath &path);
    void append(const VectorPath &path);
    void removeNode(int i);
    void calculatePercentages();
    float getLength();
    void realPercentageCalc();
    void removeNodes(int startInclusive, int endInclusive);
    float getSubSectionLength(int startIncl, int endIncl);
protected:
    std::vector <VectorPathNode> pathNodes;
};
*/

struct InterpolatedVectorData
{
    /*InterpolatedVectorData()
    {
        pingPong = false;
        loop = 0;
        //pathTimer = 0;
        //pathTime = 0;
        //pathSpeed = 1;
        //pathTimeMultiplier = 1;
        timePassed = 0;
        timePeriod = 0;
        ease = false;
        //followingPath = false;
    }*/

    Vector from;
    Vector target;

    //VectorPath path;

    int loop;

    //float pathTimer, pathTime;
    //float pathSpeed;
    //float pathTimeMultiplier;
    float timePassed, timePeriod;

    bool pingPong;
    bool ease;
    //bool followingPath;
};

// Not quite true, but they are always explicitly set anyway.
XMEM_DEFINE_POD_TYPE(InterpolatedVectorData);

enum LerpType
{
    LERP_NONE = 0,
    LERP_EASE,
    LERP_EASEIN,
    LERP_EASEOUT,
};

// This struct is used to keep all of the interpolation-specific data out
// of the global InterpolatedVector class, so that we don't waste memory on
// non-interpolated vectors.
class InterpolatedVector : public Vector
{
public:
    InterpolatedVector(scalar_t a = 0, scalar_t b = 0, scalar_t c = 0) : Vector(a,b,c), _data(0) {}
    InterpolatedVector(const Vector &vec) : Vector(vec), _data(0) {}
    ~InterpolatedVector();

    InterpolatedVector(const InterpolatedVector &vec)
        : Vector(vec)
        , _data(vec._data ? allocData(*vec._data) : 0)
    {
    }

    InterpolatedVector &operator=(const InterpolatedVector &vec);

    float interpolateTo (const Vector& vec, float timePeriod, int loopType = 0, bool pingPong = false, bool ease = false);

    void inline update(float dt)
    {
        if (!_data)
            return;

        /*if (isFollowingPath())
        {
            _updatePath(dt);
        }*/
        if (isInterpolating())
        {
            _doInterpolate(dt);
        }
    }

    inline bool isInterpolating() const
    {
        // Assume data are instantly deleted after interpolation is over
        return !!_data;
    }

    /*void startPath(float time);
    void stopPath();
    void resumePath();*/

    void stop();

    /*inline void stopAll()
    {
        stop();
        stopPath();
    }*/

    float getPercentDone() const;

    /*inline bool isFollowingPath() const
    {
        return data && data->followingPath;
    }*/

protected:

    // We never allocate this if the vector isn't used for
    // interpolation, which saves a _lot_ of memory.
    InterpolatedVectorData *_data;


    void _doInterpolate(float dt);
    void _updatePath(float dt);

    inline InterpolatedVectorData *ensureData()
    {
        if (!_data)
            _data = allocData();
        return _data;
    }

    InterpolatedVectorData *allocData();
    InterpolatedVectorData *allocData(const InterpolatedVectorData& d);
};


Vector lerp(const Vector &v1, const Vector &v2, float dt, int lerpType);


class UV
{
public:
    inline UV() : u(0), v(0) {}
    inline UV(scalar_t a, scalar_t b) : u(a), v(b) {}
    inline UV(const UV& uv) : u(uv.u), v(uv.v) {}
    inline UV(const Vector& vec) : u(vec.x), v(vec.y) {}
    inline UV operator+(const UV& uv) const { return UV(u + uv.u, v + uv.v); }
    inline UV operator-(const UV& uv) const { return UV(u - uv.u, v - uv.v); }
    inline UV operator*(const scalar_t x) const { return UV(u * x, v * x); }
    inline UV operator/(const scalar_t x) const { return *this * (1 / x); }
    inline UV operator-() const { return UV(-u, -v); }

    inline UV& operator+=(const UV& uv) { u += uv.u; v += uv.v; return *this; }
    inline UV& operator-=(const UV& uv) { u -= uv.u; v -= uv.v; return *this; }
    inline UV& operator*=(const scalar_t x) { u *= x; v *= x; return *this; }
    inline UV& operator/=(const scalar_t x) { scalar_t i = 1 / x; u *= i; v *= i; return *this; }

    scalar_t u;
    scalar_t v;
};

inline UV operator*(scalar_t x, const UV& uv) { return UV(uv.u * x, uv.v * x); }
inline UV operator/(scalar_t x, const UV& uv) { scalar_t i = 1 / x; return UV(uv.u * i, uv.v * i); }


#endif // VECTOR_H
