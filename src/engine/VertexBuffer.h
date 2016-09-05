#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include "refcounted.h"

class VertexBuffer : public Refcounted
{
public:
    VertexBuffer();
    ~VertexBuffer();
    void alloc(const void *p, unsigned bytes);
    void upload(const void *p, unsigned bytes, unsigned offset = 0);
    inline unsigned id() const { return _id; }
private:
    unsigned _id;
    unsigned _cap;
public:
    unsigned used;
};


#endif
