#ifndef MEMORY_TRACKING_POLICY_H
#define MEMORY_TRACKING_POLICY_H


class NoMemoryTracking
{
public:
    inline void OnAllocation(void*, size_t, size_t, const XSourceInfo&) const {}
    inline void OnDeallocation(void*) const {}
};



#endif

