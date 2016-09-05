#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include "Texture.h"


class SpriteSheet : public Refcounted
{
public:
    struct Def
    {
        unsigned texIdx; // index in texs
        float u1, v1, u2, v2;
        unsigned x, y, w, h;
    };

    SpriteSheet();
    ~SpriteSheet();
    int addTexAutoTile(const char *name, unsigned sx, unsigned sy);
    inline const Def& getDef(unsigned id) const { return defs[id]; }
    inline const Texture& getTex(const Def& d) { return *texs[d.texIdx]; }
    //void loadDef(const char *file);

private:
    std::vector<Def> defs;
    std::vector<CountedPtr<Texture> > texs;

};

#endif
