#ifndef ENTITY_H
#define ENTITY_H

#include "Quad.h"

class Entity : public Quad
{
public:
    Entity();
    virtual ~Entity();

    inline void setPauseLevel(int level) { _pauseLevel = level; }
    virtual bool isPaused() const;

protected:

    int _pauseLevel;
};

#endif
