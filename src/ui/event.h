#pragma once


enum WindowEventType
{
    WEV_FOCUS,
    WEV_LOSTFOCUS,
};

class IEventRecv
{
public:
    virtual void mouseMoved(unsigned x, unsigned y, int rx, int ry, unsigned buttons) = 0;
    virtual void mouseClick(unsigned button, unsigned state, unsigned x, unsigned y) = 0;
    virtual void mouseWheel(int change) = 0;
    virtual void keyEvent(unsigned key, unsigned mod, unsigned state) = 0;
    virtual void windowEvent(WindowEventType type) = 0;
    virtual void windowResize(unsigned newx, unsigned newy) = 0;
};
