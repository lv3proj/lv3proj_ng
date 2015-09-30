#ifndef TEXTURE_BASE_H
#define TEXTURE_BASE_H

#include "Resource.h"

class Texture : public Resource
{
protected:
    Texture(const char *name);
    virtual ~Texture();

public:
   inline void apply() const
   {
       if(s_lastApplied != this)
       {
           doApply();
           s_lastApplied = this;
       }
   }

   inline int getWidth() const { return width; }
   inline int getHeight() const { return height; }
   inline float getHalfWidth() const { return halfWidth; }
   inline float getHalfHeight() const { return halfHeight; }

   virtual bool reload() = 0;

   static void clearLastApplied() { s_lastApplied = NULL; }

protected:
    virtual void doApply() const = 0;

    static const Texture *s_lastApplied;

    int width, height;
    float halfWidth, halfHeight;
};

#endif
