#ifndef INTERPOLATOR_H
#define INTERNPOLATOR_H

// Explicit instantiations are in the c++ file

enum InterpolationFlags
{
    INTERP_NOFLAGS,
    INTERP_PINGPONG,
};

enum InterpolationType
{
    INTERP_LINEAR,
    INTERP_LINEAR_EASEIN,
    INTERP_LINEAR_EASEOUT,
    INTERP_LINEAR_EASEINOUT,
};

template<typename T>
struct InterpolationData
{
    T from;
    T to;
    int loop;
    float timePassed, timePeriod;
    unsigned char type;
    unsigned char flags;
};

template<typename T>
class Interpolated
{
public:
    Interpolated();
    Interpolated(const T& val);
    Interpolated(const Interpolated&);
    ~Interpolated();

    Interpolated& operator= (const Interpolated& o);
    inline bool isInterpolating() const
    {
        return !!_data;
    }
    inline void stop()
    {
        freeData();
    }
    inline void update(float dt)
    {
        if(_data)
            _interpUpdate(dt);
    }
    inline operator T() const { return v; }

    float interpolateTo(const T& to, float t, int loop = 0, bool pingpong = false, InterpolationType ty = INTERP_LINEAR);

private:

    T v;

    typedef InterpolationData<T> Data;

    inline Data *ensureData()
    {
        if (!_data)
            _data = allocData();
        return _data;
    }

    void _interpUpdate(float dt);
    static Data *allocData();
    static Data *allocData(const Data& dat);
    void freeData();
    static void _interpTo(Data *data, const T& to, float t, int loop, InterpolationFlags flags, InterpolationType ty);

    Data *_data;
};



#endif

