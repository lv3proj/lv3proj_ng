#ifndef MEMORY_TRACKING_POLICY_H
#define MEMORY_TRACKING_POLICY_H

#include "log.h"

class NoMemoryTracking
{
public:
    //               plainMemory, newSize, alignment, sourceInfo
    inline void OnAllocation(void*, size_t, size_t, const XSourceInfo&) const {}
    inline void OnDeallocation(void*, size_t) const {}

    inline size_t GetActiveAllocations() const { return 0; }
    inline size_t GetActiveBytes() const { return 0; }

    inline int GetAllocationsDelta() const { return 0; }
    inline int GetBytesDelta() const { return 0; }
    inline int GetAllocationsFreedDelta() const { return 0; }
    inline int GetBytesFreedDelta() const { return 0; }
    inline void ResetDelta() const {}
};

class SimpleMemoryTracking
{
public:
    SimpleMemoryTracking()
        : _aliveB(0), _alive(0), _iAlive(0), _iAliveB(0), _rAlive(0), _rAliveB(0)
    {
    }

    inline void OnAllocation(void*, size_t sz, size_t, const XSourceInfo&)
    {
        _aliveB += sz;
        _iAliveB += sz;
        ++_alive;
        ++_iAlive;
    }
    inline void OnDeallocation(void*, size_t sz)
    {
        _aliveB -= sz;
        _rAliveB += sz;
        --_alive;
        ++_rAlive;
    }

    inline size_t GetActiveAllocations() const { return _alive; }
    inline size_t GetActiveBytes() const { return _aliveB; }

    inline int GetAllocationsDelta() const { return _iAlive; }
    inline int GetBytesDelta() const { return _iAliveB; }
    inline int GetAllocationsFreedDelta() const { return _rAlive; }
    inline int GetBytesFreedDelta() const { return _rAliveB; }
    inline void ResetDelta() const { _iAlive = _iAliveB = _rAlive = _rAliveB = 0; }

private:
    size_t _aliveB, _alive;
    mutable int _iAlive, _iAliveB;
    mutable int _rAlive, _rAliveB;
};

template<typename T> class PrintMemoryTrackingWrapper
{
public:
    //               plainMemory, newSize, alignment, sourceInfo
    inline void OnAllocation(void *p, size_t sz, size_t aln, const XSourceInfo& src)
    {
        logdev("** Memtrack: Alloced %p, size %u, alignment %u, in %s:%u [%s] **\n", p, sz, aln, src.file, src.line, src.func);
        _piggy.OnAllocation(p, sz, aln, src);
    }
    inline void OnDeallocation(void *p, size_t sz)
    {
        logdev("** Memtrack: Freeing %p, size %u **\n", p, sz);
        _piggy.OnDeallocation(p, sz);
    }

    inline size_t GetActiveAllocations() const { return _piggy.GetActiveAllocations(); }
    inline size_t GetActiveBytes() const { return _piggy.GetActiveBytes(); }

    inline int GetAllocationsDelta() const { return _piggy.GetAllocationsDelta(); }
    inline int GetBytesDelta() const { return _piggy.GetBytesDelta(); }
    inline int GetAllocationsFreedDelta() const { return _piggy.GetAllocationsFreedDelta(); }
    inline int GetBytesFreedDelta() const { return _piggy.GetBytesFreedDelta(); }
    inline void ResetDelta() const { _piggy.ResetDelta(); }

    T _piggy;
};

#endif

