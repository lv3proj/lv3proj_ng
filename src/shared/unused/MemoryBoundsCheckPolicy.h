#ifndef MEMORY_BOUNDS_CHECK_POLICY_H
#define MEMORY_BOUNDS_CHECK_POLICY_H

class NoBoundsChecking
{
public:
    static const size_t SIZE_FRONT = 0;
    static const size_t SIZE_BACK = 0;

    inline void GuardFront(void*) const {}
    inline void GuardBack(void*) const {}

    inline void CheckFront(const void*) const {}
    inline void CheckBack(const void*) const {}
};

class SimpleBoundsChecking
{
public:
    static const size_t SIZE_FRONT = 4;
    static const size_t SIZE_BACK = 4;

    inline void GuardFront(void *p) const
    {
        *((unsigned int*)p) = 0xddccbbaa;
    }
    inline void GuardBack(void *p) const
    {
        *((unsigned int*)p) = 0xaabbccdd;
    }

    inline void CheckFront(const void *p) const
    {
        ASSERT(*((unsigned int*)p) == 0xddccbbaa);
    }
    inline void CheckBack(const void *p) const
    {
        ASSERT(*((unsigned int*)p) == 0xaabbccdd);
    }
};

class SimpleBoundsChecking2
{
public:
    static const size_t SIZE_FRONT = 4;
    static const size_t SIZE_BACK = 4;

    inline void GuardFront(void *p) const
    {
        *((unsigned int*)p) = 0x77777777;
    }
    inline void GuardBack(void *p) const
    {
        *((unsigned int*)p) = 0x99999999;
    }

    inline void CheckFront(const void *p) const
    {
        ASSERT(*((unsigned int*)p) == 0x77777777);
    }
    inline void CheckBack(const void *p) const
    {
        ASSERT(*((unsigned int*)p) == 0x99999999);
    }
};



#endif
