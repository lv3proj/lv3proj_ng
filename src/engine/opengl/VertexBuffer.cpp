#include "VertexBuffer.h"
#include "opengl/glapi.h"

VertexBuffer::VertexBuffer()
: _id(0), _cap(0), used(0)
{
    glGenBuffersARB(1, &_id);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffersARB(1, &_id);
}

void VertexBuffer::alloc(const void *p, unsigned bytes)
{
    _cap = bytes;
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, _id);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, bytes, p, GL_STATIC_DRAW_ARB);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

void VertexBuffer::upload(const void *p, unsigned bytes, unsigned offset /* = 0 */)
{
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, _id);
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, offset, bytes, p);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}


