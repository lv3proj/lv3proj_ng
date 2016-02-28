#ifndef FRAMETIME_H
#define FRAMETIME_H

class FrameTime
{
public:
    FrameTime();
    void start();
    unsigned getDiffMS() const;

protected:
    unsigned _t;

};


#endif
