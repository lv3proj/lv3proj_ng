#ifndef COLLISION_H
#define COLLISION_H

class Collidable;
class AABB;
class Circle;
class PixMap;
class Vector;

class Collision
{
private:
    Collision(); // forbid instancing - used as a namespace only.

public:
    static bool Check(const Collidable& a, const Collidable& b, Vector *v);

    static bool AABB_vs_AABB(const AABB& a, const AABB& b, Vector *v);
    static bool AABB_vs_Circle(const AABB& a, const Circle& b, Vector *v);
    static bool AABB_vs_Pixmap(const AABB& a, const PixMap& b, Vector *v);

    static bool Circle_vs_Circle(const Circle& a, const Circle& b, Vector *v);
    static bool Circle_vs_Pixmap(const Circle& a, const PixMap& b, Vector *v);

    static bool Pixmap_vs_Pixmap(const PixMap& a, const PixMap& b, Vector *v);
};

#endif
