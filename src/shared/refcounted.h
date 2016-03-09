#ifndef REFCOUNTED_H
#define REFCOUNTED_H

#include <assert.h>

class Refcounted
{
public:
    Refcounted()
    {
        _refcount = 0;
    }

    virtual ~Refcounted()
    {
        assert(_refcount == 0);
    }

    inline unsigned refcount() const { return _refcount; }

    inline void incref()
    {
        ++_refcount;
    }
    inline void decref()
    {
        unsigned oldref = _refcount;
        --_refcount;
        if (oldref <= 1)
        {
            // if the refcount is now zero, it will stay zero forever as nobody has a reference anymore
            delete this;
        }
    }

private:
    unsigned _refcount;
};


template<typename T> class CountedPtr
{
public:
    inline ~CountedPtr()
    {
        if(_p)
            _p->decref();
    }
    inline CountedPtr() : _p(0)
    {}
    inline CountedPtr(T* p) : _p(p)
    {
        if(p)
            p->incref();
    }
    inline CountedPtr(const CountedPtr& ref) : _p(ref._p)
    {
        if (_p)
            _p->incref();
    }
    // intentionally not a reference -- see http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    CountedPtr& operator=(CountedPtr ref)
    {
        CountedPtr::swap(*this, ref);
        return *this;
    }

    const T* operator->() const  { return _p; }
          T* operator->()        { return _p; }

    bool operator!() const { return !_p; }

    // Safe for use in if statements
    operator void*() const  { return _p; }

    // if you use these, make sure you also keep a counted reference to the object!
    T* content ()       { return _p; }
    const T* content () const { return _p; }

    bool operator<(const CountedPtr& ref) const { return _p < ref._p; }
    bool operator<=(const CountedPtr& ref) const { return _p <= ref._p; }
    bool operator==(const CountedPtr& ref) const { return _p == ref._p; }
    bool operator!=(const CountedPtr& ref) const { return _p != ref._p; }
    bool operator>=(const CountedPtr& ref) const { return _p >= ref._p; }
    bool operator>(const CountedPtr& ref) const { return _p > ref._p; }

    bool operator<(const T *p) const { return _p < p; }
    bool operator<=(const T *p) const { return _p <= p; }
    bool operator==(const T *p) const { return _p == p; }
    bool operator!=(const T *p) const { return _p != p; }
    bool operator>=(const T *p) const { return _p >= p; }
    bool operator>(const T *p) const { return _p > p; }

    inline static void swap(CountedPtr& a, CountedPtr& b)
    {
        std::swap(a._p, b._p);
    }

private:

    T *_p;
};

#endif
