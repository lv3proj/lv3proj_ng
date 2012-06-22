#ifndef SHAPE_PIXMAP_H
#define SHAPE_PIXMAP_H

#include "Collidable.h"
#include "array2d.h"

class AABB;

class PixMap : public Collidable
{

public:
    PixMap()
        : Collidable(COLL_CIRCLE)
    {
    }

    PixMap(const PixMap& o)
        : Collidable(o)
        , position(o.position)
        , pix(o.pix)
    {
    }

    virtual AABB getAABB() const;

    Vector position;
    array2d<unsigned char> pix;
};

#endif
