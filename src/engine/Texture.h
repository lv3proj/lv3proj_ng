#ifndef TEXTURE_BASE_H
#define TEXTURE_BASE_H

#include "Resource.h"

class Texture : public Resource
{
protected:
    Texture(const char *name);
    virtual ~Texture();

public:
   inline void apply()
   {
       if(s_lastApplied != this)
       {
           doApply();
           s_lastApplied = this;
       }
   }

   inline int getWidth() const { return width; }
   inline int getHeight() const { return height; }

   virtual bool reload() = 0;

protected:
    virtual void doApply() = 0;

    static Texture *s_lastApplied;

    int width, height;
};

#endif
