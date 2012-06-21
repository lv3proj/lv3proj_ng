#ifndef ARRAY2D_H
#define ARRAY2D_H

#include <assert.h>


// fast 2D array avoiding multiplication to access array indexes
// the size has to be a power of 2, if not, it will automatically align
template <class T> class array2d
{
public:

    array2d() : _shift(0), _size(0), data(NULL) {} // when checking for bounds, we NEED a default value to return
    ~array2d() { this->free(); }

    inline void fill(T val)
    {
        uint32 s = size2d();
        for(uint32 i = 0; i < s; ++i)
            data[i] = val;
    }

    inline void free(void)
    {
        if(data)
        {
            delete [] data;
            data = NULL;
            _shift = _size = 0;
        }
    }

    inline void resize(unsigned int dim, const T& fillval, bool force = false)
    {
        unsigned int req = 0;
        unsigned int newsize = 1;

        // find out how often we have to shift to reach the desired capacity
        // this will set the size to the nearest power of 2 required (if dim is 50 the final size will be 64, for example)
        while(newsize < dim)
        {
            newsize <<= 1;
            ++req;
        }

        // array does already have the desired size and data field, nothing to do
        if(!force && newsize == _size && data)
            return;

        // save old size and data field for later copy
        uint32 oldsize = _size;
        T* olddata = data;

        // alloc new space
        _size = newsize;
        data = new T[_size * _size];

        // fill it up to prevent uninitialized memory
        fill(fillval);

        // if there was content, copy it
        if(olddata && oldsize)
        {
            unsigned int copysize = oldsize < newsize ? oldsize : newsize;
            for(unsigned int x = 0; x < copysize; ++x)
                for(unsigned int y = 0; y < copysize; ++y)
                    data[(y << req) | x] = olddata[(y << _shift) | x];
            delete [] olddata;
        }

        _shift = req;
    }

    inline T& operator () (unsigned int x, unsigned int y)
    {
        assert(x < _size && y < _size);
        return data[(y << _shift) | x];
    }

    inline const T& operator () (unsigned int x, unsigned int y) const
    {
        assert(x < _size && y < _size);
        return data[(y << _shift) | x];
    }

    inline T& operator [] (unsigned int pos)
    {
        return data[pos];
    }

    inline const T& operator [] (unsigned int pos) const
    {
        return data[pos];
    }

    inline T* array(void)
    {
        return &data[0];
    }

    inline const T* array(void) const
    {
        return &data[0];
    }

    inline unsigned int size1d(void) const { return _size; }
    inline unsigned int size2d(void) const { return _size * _size; }

    // use at your own risk
    inline T* getPtr(void) { return data; }
    inline const T* getPtr(void) const { return data; }
    inline void setPtr(T *p) { data = p; }
    inline void resizeNoAlloc(unsigned int s)
    {
        _shift = 0;
        _size = 1;
        while(_size < s)
        {
            _size <<= 1;
            ++_shift;
        }
    }


protected:

    unsigned int _size;
    unsigned int _shift;

    T *data;

};

#endif
