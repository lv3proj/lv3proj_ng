#ifndef INTERVAL_TIMER_H
#define INTERVAL_TIMER_H

class IntervalTimer
{
protected:
    float _interval;
    float _cur;

public:
    IntervalTimer(float t = 0)
        : _interval(t), _cur(t)
    {
    }

    void set(float t)
    {
        _interval = t;
        _cur = t;
    }
    
    bool update(float dt)
    {
        _cur = _cur - dt;
        if(_cur <= 0)
        {
            _cur += _interval;
            return true;
        }
        return false;
    }

};


#endif
