#ifndef COLLISION_H
#define COLLISION_H

class Collidable;
class AABB;
class Circle;
class Vector;
class Line;

enum CollisionShape
{
    COLL_CIRCLE,
    COLL_AABB,
    COLL_LINE,
    // TODO: Polygon = ?x line collision /w point point transformation/rotation
};

class Collision
{
private:
    Collision(); // forbid instancing - used as a namespace only.

public:
    static bool Check(const Collidable& a, const Collidable& b, Vector *v);

    static bool AABB_vs_AABB(const AABB& a, const AABB& b, Vector *v);
    static bool AABB_vs_Circle(const AABB& a, const Circle& b, Vector *v);
    static bool AABB_vs_Line(const AABB& a, const Line& b, Vector *v);

    static bool Circle_vs_Circle(const Circle& a, const Circle& b, Vector *v);
    static bool Circle_vs_Line(const Circle& a, const Line& b, Vector *v);

    static bool Line_vs_Line(const Line& a, const Line& b, Vector *v);
};

#endif
