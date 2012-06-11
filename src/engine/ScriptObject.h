#ifndef SCRIPT_OBJECT_H
#define SCRIPT_OBJECT_H

class ObjectMgr;


enum ScriptObjectType
{
    OT_NONE         = 0x00000000,
    OT_RENDEROBJECT = 0x00000001,
    OT_QUAD         = 0x00000002 | OT_RENDEROBJECT,
    OT_SOUND        = 0x00000004,
};

class ScriptObject
{
    friend class ObjectMgr;

protected:
    ScriptObject();
    virtual ~ScriptObject();

public:

    virtual void update(float dt) {}

    void *scriptBindings;

    inline bool isDead() const { return _dead; }
    inline bool isManaged() const { return _managed; }

protected:

    bool _dead;
    bool _managed; // If true, object will go to objmgr.
};


class LifeObject : public ScriptObject
{
public:
    LifeObject();
    virtual ~LifeObject();

    virtual void update(float dt);
    void kill(float decay = -1);

protected:

    virtual void onEndOfLife();

private:

    float _life;
    float _decay;
};

#endif
