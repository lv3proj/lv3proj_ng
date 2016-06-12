#ifndef MEMORY_THREAD_POLICY
#define MEMORY_THREAD_POLICY

class SingleThreadPolicy
{
public:
    inline void Enter() const {}
    inline void Leave() const {}
};

template <class TMUTEX>
class MultiThreadPolicy
{
public:
    inline void Enter()
    {
        m_primitive.Lock();
    }

    inline void Leave()
    {
        m_primitive.Unlock();
    }

private:
    TMUTEX m_primitive;
};

#endif
