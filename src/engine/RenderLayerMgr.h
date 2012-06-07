#ifndef RENDERLAYERMGR_H
#define RENDERLAYERMGR_H


#include <vector>
#include <map>

class RenderLayer;

class RenderLayerMgr
{
public:
    RenderLayerMgr();
    ~RenderLayerMgr();

    void ClearAll();
    void Render();

    inline RenderLayer *GetLayer(unsigned int id) { return _layers[id]; }
    RenderLayer *GetLayer(const char *name);


private:
    std::vector<RenderLayer*> _layers;
};


#endif
