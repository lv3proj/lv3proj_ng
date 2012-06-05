#ifndef SCRIPT_OBJECT_H
#define SCRIPT_OBJECT_H

class ObjectMgr;


enum ScriptObjectType
{
    OT_NONE         = 0x00000000,
    OT_RENDEROBJECT = 0x00000001,
    OT_QUAD         = 0x00000002 | OT_RENDEROBJECT,
};


class ScriptObject
{
    friend class ObjectMgr;

protected:
    ScriptObject();
    virtual ~ScriptObject();

public:

    virtual void update(float dt);
    void kill(float decay = -1);

    void *scriptBindings;

    inline  bool isDead() const { return _dead; }


protected:

    virtual void onEndOfLife();

private:

    float _life;
    float _decay;
    bool _dead;

};


#endif
