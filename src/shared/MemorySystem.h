#ifndef MEMORYSYSTEM_H
#define MEMORYSYSTEM_H

//Based on Molecule engine's memory allocation system
// http://molecularmusings.wordpress.com

#include "MemoryArena.h"


#define XNEW(ty, a)    new (_X_New(sizeof(ty), a, __FILE__, __LINE__)) ty
//#define XNEW(ty)       new ty

#define XDELETE(ty, a) _X_Delete(ty, a)
#define XDELETE_NN(ty, a) _X_DeleteUnchecked(ty, a)
//#define XDELETE(ty)    delete ty

#define XNEW_ARRAY(type, arena)    _X_NewArray<TypeAndCount<type>::Type>(arena, TypeAndCount<type>::Count, __FILE__, __LINE__, IntToType<XIS_POD(TypeAndCount<type>::Type)>())

#define XDELETE_ARRAY(type, arena)    _X_DeleteArray(type, arena)

template <class ARENA>
inline void *_X_New(size_t size, ARENA& arena, const char *file, size_t line)
{
    // default alignment by pointer size
    return  arena.Allocate(size, sizeof(void*), XSourceInfo(__FILE__, __LINE__));
}


template <typename T, class ARENA>
inline void _X_Delete(T* object, ARENA& arena)
{
    if(!object)
        return;

    // call the destructor first...
    object->~T();

    // ...and free the associated memory
    arena.Free(object);
}

template <typename T, class ARENA>
inline void _X_DeleteUnchecked(T* object, ARENA& arena)
{
    // call the destructor first...
    object->~T();

    // ...and free the associated memory
    arena.Free(object);
}


template <typename T, class ARENA>
inline T* _X_NewArray(ARENA& arena, size_t N, const char* file, size_t line, NonPODType)
{
    union
    {
        void* as_void;
        size_t* as_size_t;
        T* as_T;
    };

    as_void = arena.Allocate(sizeof(T)*N + sizeof(size_t), XSourceInfo(file, line));

    // store number of instances in first size_t bytes
    *as_size_t++ = N;

    // construct instances using placement new
    const T* const onePastLast = as_T + N;
    while (as_T < onePastLast)
        new (as_T++) T;

    // hand user the pointer to the first instance
    return (as_T - N);
}

template <typename T, class ARENA>
inline T* _X_NewArray(ARENA& arena, size_t N, const char* file, size_t line, PODType)
{
    return static_cast(arena.Allocate(sizeof(T)*N, XSourceInfo(file, line)));
}

template <typename T, class ARENA>
void _X_DeleteArray(T* ptr, ARENA& arena, PODType)
{
    union
    {
        size_t* as_size_t;
        T* as_T;
    };

    // user pointer points to first instance...
    as_T = ptr;

    // ...so go back size_t bytes and grab number of instances
    const size_t N = as_size_t[-1];

    // call instances' destructor in reverse order
    for (size_t i=N; i>0; --i)
        as_T[i-1].~T();

    arena.Free(as_size_t-1);
}

template <typename T, class ARENA>
inline void _X_DeleteArray(T* ptr, ARENA& arena, NonPODType)
{
    arena.Free(ptr);
}

template <typename T, class ARENA>
inline void _X_DeleteArray(T* ptr, ARENA& arena)
{
    _X_DeleteArray(ptr, arena, IntToType<XIS_POD(T)>());
}



#endif
