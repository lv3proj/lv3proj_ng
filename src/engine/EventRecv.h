#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <vector>

class MouseEventRecv;
class KeyboardEventRecv;
class WindowEventRecv;
class JoystickEventRecv;
class RawEventRecv;

class EventRecv
{
public:
    EventRecv();
    ~EventRecv();

    void Update();
    void Clear();

    void Add(MouseEventRecv *ev) { mouse.push_back(ev); }
    void Add(KeyboardEventRecv *ev) { kb.push_back(ev); }
    void Add(WindowEventRecv *ev) { win.push_back(ev); }
    void Add(JoystickEventRecv *ev) { joy.push_back(ev); }
    void Add(RawEventRecv *ev) { raw.push_back(ev); }

private:

    std::vector<MouseEventRecv*> mouse;
    std::vector<KeyboardEventRecv*> kb;
    std::vector<WindowEventRecv*> win;
    std::vector<JoystickEventRecv*> joy;
    std::vector<RawEventRecv*> raw;
};

#endif

