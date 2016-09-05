#ifndef ARRAY2D_H
#define ARRAY2D_H

#include <assert.h>
#include <stdlib.h>
#include "bithacks.h"



// fast 2D array avoiding multiplication to access array indexes
// the size has to be a power of 2, if not, it will automatically align
template <typename T> class FastArray2D
{
private:
    T *data;
    unsigned _shift;
    unsigned _w, _h;

public:
    FastArray2D() : data(NULL), _shift(0), _w(0), _h(0) {}
    FastArray2D(const FastArray2D<T>& o) : _shift(0), _w(0), _h(0), data(0) // w, h inited by resize - must be 0 here!
    {
        resize(o._w, o._h);
        for(size_t y = 0; y < _h; ++y)
            for(size_t x = 0; x < _w; ++x)
                (*this)(x, y) = o(x, y);
    }
    ~FastArray2D() { clear(); }

    inline unsigned width() const { return _w; }
    inline unsigned height() const { return _h; }

    void fill(const T& val)
    {
        const unsigned h = _h;
        for(unsigned y = 0; y < h; ++y)
        {
            T *p = &(*this)(0, y);
            T * const end = p + _w;
            for( ; p < end; ++p )
                *p = val;
        }
    }

    void clear()
    {
        delete [] data;
        data = NULL;
        _shift = _w = _h = 0;
    }

    void resize(unsigned w, unsigned h, const T& val = T())
    {
        if(!w && !h)
        {
            clear();
            return;
        }
        if(w == _w && h == _h)
            return;

        unsigned shift = 0;
        unsigned ww = 1;
        while(ww < w)
        {
            ww <<= 1;
            ++shift;
        }

        // save old size and data field for later copy
        const unsigned oldw = _w;
        const unsigned oldh = _h;
        const unsigned oldshift = _shift;
        T * const olddata = data;

        // alloc new space
        _w = w;
        _h = h;
        _shift = shift;
        data = new T[ww * h];

        fill(val);

        // if there was content, copy it
        if(olddata && oldw && oldh)
        {
            const unsigned copyw = oldw < w ? oldw : w;
            const unsigned copyh = oldh < h ? oldh : h;
            for(unsigned y = 0; y < copyh; ++y)
            {
                const T *rp = &olddata[y << oldshift];
                T *wp = &data[y << shift];
                for(unsigned x = 0; x < copyw; ++x)
                    *wp++ = *rp++;
                delete [] olddata;
            }
        }
    }

    inline T& operator () (unsigned x, unsigned y)
    {
        assert(x < _w && y < _h);
        return data[(y << _shift) | x];
    }

    inline const T& operator () (unsigned x, unsigned y) const
    {
        assert(x < _w && y < _h);
        return data[(y << _shift) | x];
    }

/*
    // intentionally not a reference -- see http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    FastArray2D<T>& operator= (FastArray2D<T> o)
    {
        FastArray2D::swap(*this, o);
        return *this;
    }

    static void swap(FastArray2D<T>& a1, FastArray2D<T>& a2)
    {
        std::swap(a1._mem, a2._mem);
        std::swap(a1._w, a2._w);
        std::swap(a1._h, a2._h);
        std::swap(a1._shift, a2._shift);
    }
*/


private:
    FastArray2D<T>& operator= (FastArray2D<T> o); // non-copyable
};

#endif
