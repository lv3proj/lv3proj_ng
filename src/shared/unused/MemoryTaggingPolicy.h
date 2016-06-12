#ifndef MEMORY_TAGGING_POLICY_H
#define MEMORY_TAGGING_POLICY_H


class NoMemoryTagging
{
public:
    inline void TagAllocation(void*, size_t) const {}
    inline void TagDeallocation(void*, size_t) const {}
};

class DeadFillTagging
{
public:
    inline void TagAllocation(void *p, size_t s) const { memset(p, 0xed, s); }
    inline void TagDeallocation(void *p, size_t s) const { memset(p, 0xfe, s); }
};

class DeadFillTagging2
{
public:
    inline void TagAllocation(void *p, size_t s) const { memset(p, 0x88, s); }
    inline void TagDeallocation(void *p, size_t s) const { memset(p, 0x55, s); }
};


#endif
