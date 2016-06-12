#pragma once


enum WindowEventType
{
    WEV_FOCUS,
    WEV_LOSTFOCUS,
    WEV_QUIT,
};

class MouseEventRecv
{
public:
    virtual void mouseMoved(unsigned which, unsigned x, unsigned y, int rx, int ry, unsigned buttons) = 0;
    virtual void mouseClick(unsigned which, unsigned button, unsigned state, unsigned x, unsigned y) = 0;
    virtual void mouseWheel(unsigned which, int change) = 0;
};

class KeyboardEventRecv
{
public:
    virtual void keyEvent(unsigned key, unsigned mod, unsigned state) = 0;
};

class WindowEventRecv
{
public:
    virtual void windowEvent(WindowEventType type) = 0;
    virtual void windowResize(unsigned newx, unsigned newy) = 0;
};

class JoystickEventRecv
{
public:
    virtual void axisChange(unsigned which, unsigned axis, float val) = 0;
    virtual void button(unsigned which, unsigned id, unsigned state) = 0;
    //virtual void hat(unsigned which, unsigned id, unsigned state) = 0;
};

class RawEventRecv
{
public:
    virtual void rawEvent(void*) = 0;
};

