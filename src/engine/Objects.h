#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "refcounted.h"
#include "glm/glm.hpp"
#include "Texture.h"

// NOTE: None of these objects have ANY virtual methods (not even a virtual dtor!)

enum ObjectType
{
    RO_GROUP,
    RO_SPRITE,
    //RO_QUAD,
    //RO_SCRIPT,
};

enum ObjectFlags
{
    OF_DEAD = 0x01,
}

// Base class, not rendered
class BaseObject
{
    friend class CountedPtr<BaseObject>;
public:
    static void _Destroy(BaseObject *);
    inline ObjectType getType() const { return ObjectType(_type); }
    inline bool isDead() const { return _flags & OF_DEAD; }
protected:
    BaseObject(ObjectType ty);
    ~BaseObject();
private:
    inline void incref();
    void decref();
    unsigned char _type;
    unsigned char _flags;
    unsigned short _refcount;
};

// Single texture, rendered. Very lightweight.
class Sprite : public BaseObject
{
public:
    static Sprite *Create();
    static void _Destroy(Sprite *);
    glm::mat4 getLocalTransform() const;
    CountedPtr<Texture> tex;
    glm::vec2 uv[4];
    glm::vec2 size; // width, height
    glm::vec2 pos;
    glm::vec4 color;
protected:
    Sprite();
    ~Sprite();
};

/*
struct InterpolationData
{
};
*/

// Single texture, rendered. Can rotate and other things
/*class Quad
{
    // TODO WRITE ME
    InterpolationData *interp;
};*/

// Collection of other objects, itself not rendered
class GroupObject : public BaseObject
{
public:
    static GroupObject *Create();
    static void _Destroy(GroupObject *);
    glm::mat4 getLocalTransform() const;
    void add(BaseObject *ro) { _ch.push_back(ro); }
    unsigned remove(BaseObject *);
    void remove(unsigned idx);
    void clear() { _ch.clear(); }
    size_t size() const { return _ch.size(); }
    BaseObject *child(size_t i) { return _ch[i].content(); }
    const BaseObject *child(size_t i) const { return _ch[i].content(); }
protected:
    typedef std::vector<CountedPtr<BaseObject> > Children;
    GroupObject();
    ~GroupObject();
    //InterpolationData *interp;
private:
    Children _ch;
};

// Collection of other objects, scripted, but itself not rendered
/*class ScriptObject : public GroupObject
{
    InterpolationData *interp;
}*/




#endif

