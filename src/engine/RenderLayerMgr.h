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

    inline RenderLayer *GetLayer(unsigned int id) const { return _layers[id]; }
    RenderLayer *GetLayer(const char *name);
    inline unsigned int GetLayerCount() const { return (unsigned int)_layers.size(); }


private:
    std::vector<RenderLayer*> _layers;
};


#endif
