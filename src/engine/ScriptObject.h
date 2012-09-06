#ifndef SCRIPT_OBJECT_H
#define SCRIPT_OBJECT_H

class ObjectMgr;


enum ScriptObjectType
{
    OT_NONE         = 0x00000000,
    OT_RENDEROBJECT = 0x00000001,
    OT_QUAD         = 0x00000002 | OT_RENDEROBJECT,
    OT_SOUND        = 0x00000004,
    OT_ENTITY       = 0x00000008 | OT_QUAD
};

class ScriptObject
{
    friend class ObjectMgr;

protected:
    ScriptObject();
    virtual ~ScriptObject();

public:

    virtual void update(float dt) {}
    virtual void updateFixed(float dt) {}

    void *scriptBindings;

    inline bool isDead() const { return _dead; }
    inline bool isManaged() const { return _managed; }

    inline void setPauseLevel(char level) { _pauseLevel = level; }
    bool isPaused() const;

protected:

    bool _dead;
    bool _managed; // If true, object will go to objmgr.
    char _pauseLevel;

};


class LifeObject : public ScriptObject
{
public:
    LifeObject();
    virtual ~LifeObject();

    virtual void update(float dt);
    virtual bool isPaused() const { return false; }

    void kill(float decay = -1);

protected:

    virtual void onEndOfLife();

private:

    float _life;
    float _decay;
};

#endif
