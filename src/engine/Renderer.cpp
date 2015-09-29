#include <SDL/SDL.h>

#include "Renderer.h"
#include "OpenGLAPI.h"
#include "GL/gl.h"
#include "Engine.h"
#include "SDLSurfaceResource.h"
#include "GLTexture.h"
#include "Quad.h"
#include "Camera.h"
#include "Tile.h"

#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049
#define TEXTURE_FREE_MEMORY_ATI 0x87FC

enum ClientState
{
    CLS_NONE = 0,
    CLS_VERT_TEXCOORD = 1,
};

Renderer::Renderer()
: screen(NULL), _clientState(0), _activeBlend(BLEND_NONE)
{
    memset(&settings, 0, sizeof(settings));
    globalResolutionScale = Vector(1, 1, 1);

    // debug stuff
    renderBorders = false;
}

Renderer::~Renderer()
{
}

bool Renderer::Init()
{
    logdebug("Renderer init...");

    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
    {
        logcritical("SDL Error: %s",SDL_GetError());
        return false;
    }

    logdebug("Loading OpenGL...");

    if (SDL_GL_LoadLibrary(NULL) == -1)
    {
        logcritical("SDL_GL_LoadLibrary Error: %s", SDL_GetError());
        return false;
    }

    logdebug("Loading OpenGL functions...");

    if(!OpenGLAPI::LoadSymbols())
        return false;

    return true;
}

unsigned int Renderer::getFreeVideoMemoryKB()
{
    GLint meminfo[4];
    memset(meminfo, 0, 4 * sizeof(GLint));

    // nvidia?
    glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, meminfo); // in KB
    if(!meminfo[0])
    {
        // ATI?
        glGetIntegerv(TEXTURE_FREE_MEMORY_ATI, meminfo); // in KB, // [0] = total memory free in the pool
    }
    GLenum err = glGetError();

    return meminfo[0];
}

void Renderer::BeginFrame()
{
    OpenGLAPI::ResetCallCount();
}

void Renderer::EndFrame()
{
}

unsigned int Renderer::GetRenderCallCount()
{
    return OpenGLAPI::GetCallCount();
}

void Renderer::Shutdown()
{
    glFinish();
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    //SDL_WM_GrabInput(SDL_GRAB_OFF);

    screen = 0;

    OpenGLAPI::ClearSymbols();
}

void Renderer::SetInternalResolution(float x, float y)
{
    settings.virtualW = x;
    settings.virtualH = y;
}

void Renderer::SetScreenResolution(uint32 x, uint32 y)
{
    settings.pixelsW = x;
    settings.pixelsH = y;
}

void Renderer::SetBPP(int bpp)
{
    settings.bpp = bpp;
}

void Renderer::SetVsync(bool on)
{
    settings.vsync = on;
}

void Renderer::SetFullscreen(bool on)
{
    settings.full = on;
}

Texture *Renderer::createTexture(const char *name)
{
    GLTexture *tex = new GLTexture(name);
    if(!tex->reload())
    {
        logerror("Failed to create OpenGL texture %s", name);
        delete tex;
        return NULL;
    }

    logdebug("Created OpenGL texture %s", name);
    return tex;
}


bool Renderer::ApplySettings()
{
    // FIXME: return early if nothing changed


    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, settings.vsync);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    Uint32 flags = SDL_OPENGL | SDL_RESIZABLE;
    if(settings.full)
        flags |= SDL_FULLSCREEN;

    logdetail("Setting video mode: %ux%u, %u bpp, %s",
        settings.pixelsW, settings.pixelsH, settings.bpp, settings.full ? "fullscreen" : "windowed");

    screen = SDL_SetVideoMode(settings.pixelsW, settings.pixelsH, settings.bpp, flags);
    if(!screen)
    {
        logerror("Failed to set resolution %ux%u", settings.pixelsW, settings.pixelsH);
        return false;
    }

    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *rend = glGetString(GL_RENDERER);
    const GLubyte *ver = glGetString(GL_VERSION);
    //const GLubyte *exts = glGetString(GL_EXTENSIONS);

    log("== Video info ==");
    log("Driver:   %s", vendor);
    log("Renderer: %s", rend);
    log("Version: %s", ver);
    //log("Extensions: %s", exts);

    glViewport(0, 0, settings.pixelsW, settings.pixelsH);
    glScissor(0, 0, settings.pixelsW, settings.pixelsH);

    glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Black Background FIXME
    glClearDepth(1.0f);								// Depth Buffer Setup
    glDisable(GL_CULL_FACE);

    glLoadIdentity();

    glFinish();

    clear();
    show();

    aspect = float(settings.pixelsW) / float(settings.pixelsH);

    if (aspect >= 1.3f)
    {
        int vw = int(float(settings.virtualH) * (float(settings.pixelsW)/float(settings.pixelsH)));
        _Enable2D(vw, settings.virtualH);
    }
    else
    {
        int vh = int(float(settings.virtualW) * (float(settings.pixelsH)/float(settings.pixelsW)));
        _Enable2D(settings.virtualW, vh);
    }

    unsigned int freemem = getFreeVideoMemoryKB();
    if(freemem)
    {
        logdetail("Free video memory: %u KB", freemem);
    }
    else
    {
        logdetail("Unable to query free video memory");
    }

    _colorstack.clear();
    _RenderColor col;
    col.r = col.g = col.b = col.a = 1;
    _colorstack.push_back(col);

    return true;
}

static void _Ortho2D(float left, float right, float bottom, float top)
{
    logdetail("Ortho2D: (L: %.2f, R: %.2f, T: %.2f, B: %.2f)", left, right, top, bottom);
    glOrtho(left, right, bottom, top, -1.0, 1.0);
}

void Renderer::_Enable2D(uint32 x, uint32 y)
{
    logdebug("Enable2D: (%u, %u)", x, y);

    GLint viewPort[4];
    glGetIntegerv(GL_VIEWPORT, viewPort);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float vw=0,vh=0;

    float viewOffX = 0, viewOffY = 0;

    float aspect = float(settings.pixelsW)/float(settings.pixelsH);

    if (aspect >= 1.4f)
    {
        vw = (settings.virtualW * viewPort[3]) / settings.virtualH;

        viewOffX = (viewPort[2] - vw) * 0.5f;
        logdebug("viewOffsX = %f", viewOffX);
    }
    else if (aspect < 1.3f)
    {
        vh = (settings.virtualH * viewPort[2]) / settings.virtualW;

        viewOffY = (viewPort[3] - vh) * 0.5f;
        logdebug("viewOffsY = %f", viewOffY);
    }

    _Ortho2D(0.0f-viewOffX,viewPort[2]-viewOffX,viewPort[3]-viewOffY,0.0f-viewOffY);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    setupRenderPositionAndScale();

    float widthFactor  = settings.pixelsW / float(x);
    float heightFactor = settings.pixelsH / float(y);
    globalResolutionScale = Vector(widthFactor,heightFactor,1.0f);

    float diff = x - settings.virtualW;
    if (diff > 0)
        engine->virtualOffX = diff / 2;
    else
        engine->virtualOffX = 0;


    diff = y - settings.virtualH;
    if (diff > 0)
        engine->virtualOffY = diff / 2;
    else
        engine->virtualOffY = 0;

    logdebug("virtualOffX = %.2f", engine->virtualOffX);
    logdebug("virtualOffY = %.2f", engine->virtualOffY);
}

void Renderer::setupRenderPositionAndScale()
{
    glLoadIdentity();
    // FIXME: not sure where to store the camera, this seems to make sense

    const Vector& globalScale = engine->camera->scale;
    const Vector& cameraPos = engine->camera->position;
    const Vector& cameraOffset = engine->camera->offset;
    //const Vector& cameraRot = engine->camera->rotation;

    //glRotatef(cameraRot.z, 0, 0, 1);
    glScalef(globalScale.x * globalResolutionScale.x,
             globalScale.y * globalResolutionScale.y,
             globalScale.z * globalResolutionScale.z);

    glTranslatef(-(cameraPos.x+cameraOffset.x), -(cameraPos.y+cameraOffset.y), -(cameraPos.z+cameraOffset.z));
    //glRotatef(180, 0, 1, 0);
}

void Renderer::setupScreenScale()
{
    glLoadIdentity();
    glScalef(globalResolutionScale.x,
             globalResolutionScale.y,
             globalResolutionScale.z);
}

void Renderer::loadIdentity()
{
    glLoadIdentity();
}


void Renderer::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
    Texture::clearLastApplied();
    _renderedObjects = 0;
    _renderedVerts = 0;
}

void Renderer::show()
{
    SDL_GL_SwapBuffers();
}


const bool DEBUG_RENDER = true;

void Renderer::renderObject(const RenderObject *ro)
{
    if(ro->alpha.x <= 0)
        return;

    const Vector renderPos = ro->getParallaxRenderPosition(engine->camera->screenCenter); //ro->position + ro->offset;
    //const Vector renderRot = ro->rotation + ro->rotation2;
    const Vector& renderCol = ro->color;
    const Vector& renderCol2 = ro->color2;

    glPushMatrix();
    glTranslatef(renderPos.x, renderPos.y, renderPos.z);

    glRotatef(ro->rotation.x, 0, 0, 1);
    /*if (isfh())
    {
    glDisable(GL_CULL_FACE);
    glRotatef(180, 0, 1, 0);
    }*/

    glScalef(ro->scale.x, ro->scale.y, 1);

    _applyBlendType(ro->getBlendType());

    _pushColor();
    _multColor(renderCol.x, renderCol.y, renderCol.z, ro->alpha.x);
    _pushColor();
    _multColor(renderCol2.x, renderCol2.y, renderCol2.z, ro->alpha2.x);
    _applyColor();

    glPushMatrix();
    glRotatef(ro->rotation2.x, 0, 0, 1);
    glScalef(ro->scale2.x, ro->scale2.y, 1);
    ro->onRender();
    glPopMatrix();

    _popColor();

    const RenderObject::Children& ch = ro->getChildren();
    for(RenderObject::Children::const_iterator it = ch.begin(); it != ch.end(); ++it)
        renderObject(*it);


    /*if(DEBUG_RENDER)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glLineWidth(4);
        glEnable(GL_BLEND);

        int i = 0;
        glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
        glVertex2f(renderPos.x, renderPos.y);
    }*/

    glPopMatrix();
    _popColor();

    ++_renderedObjects;
}

void Renderer::_applyBlendType(BlendType blend)
{
    if(_activeBlend == blend)
        return;

    switch (blend)
    {
    case BLEND_NONE:
        glDisable(GL_BLEND);
        glDisable(GL_ALPHA_TEST);
    case BLEND_DEFAULT:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case BLEND_ADD:
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
        break;
    case BLEND_SUB:
        glEnable(GL_BLEND);
        glBlendFunc(GL_ZERO, GL_SRC_ALPHA);
        break;
    case BLEND_MULT:
        glEnable(GL_BLEND);
        glBlendFunc(GL_ZERO, GL_SRC_COLOR);
        break;
    }

    _activeBlend = blend;
}

void Renderer::renderQuad(const Quad *q)
{
    Texture *tex = q->getTexture();
    if(!tex)
        return;

    tex->apply();
    const float w2 = q->getHalfWidth();
    const float h2 = q->getHalfHeight();
    const UV& ultc = q->upperLeftTextureCoords;
    const UV& lrtc = q->lowerRightTextureCoords;

    _enableVertexAndTexCoords();

    const GLfloat vertexData[] =
    {
        -w2, -h2, // upper left
        +w2, -h2, // upper right
        -w2, +h2, // lower left
        +w2, +h2, // lower right
    };
    const GLfloat texCoords[] =
    {
        ultc.u, ultc.v, // upper left
        lrtc.u, ultc.v, // upper right
        ultc.u, lrtc.v, // lower left
        lrtc.u, lrtc.v, // lower right
    };
    glVertexPointer(2, GL_FLOAT, 0, &vertexData);
    glTexCoordPointer(2, GL_FLOAT, 0, &texCoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    _renderedVerts += 4;

    if(renderBorders)
    {
        const GLfloat vertexData2[] =
        {
            -w2, -h2, // upper left
            +w2, -h2, // upper right
            +w2, +h2, // lower left
            -w2, +h2, // lower right
        };
        glVertexPointer(2, GL_FLOAT, 0, &vertexData2);
        glBindTexture(GL_TEXTURE_2D, 0);
        Texture::clearLastApplied();
        glLineWidth(1);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
        _renderedVerts += 4;
    }
}

static const GLfloat simpleTexCoords[] =
{
    0,      0, // upper left
    1,      0, // upper right
    0,      1, // lower right
    1,      1, // lower left
};

void Renderer::renderSingleTexture(Texture *tex, const Vector& pos)
{
    Vector halfsize(tex->getHalfWidth(), tex->getHalfHeight());
    renderSingleTexture(tex, pos, halfsize);
}

void Renderer::renderSingleTexture(Texture *tex, const Vector& pos, const Vector& halfsize)
{
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);

    tex->apply();
    const float w2 = halfsize.x;
    const float h2 = halfsize.y;

    /*glBegin(GL_QUADS);
    {
        glTexCoord2f(0, 1);
        glVertex2f(-w2, +h2);

        glTexCoord2f(1, 1);
        glVertex2f(+w2, +h2);

        glTexCoord2f(1, 0);
        glVertex2f(+w2, -h2);

        glTexCoord2f(0, 0);
        glVertex2f(-w2, -h2);
    }
    glEnd();*/

    _enableVertexAndTexCoords();

    const GLfloat vertexData[] =
    {
        -w2, -h2, // upper left
        +w2, -h2, // upper right
        -w2, +h2, // lower right
        +w2, +h2, // lower left
    };
    glVertexPointer(2, GL_FLOAT, 0, &vertexData);
    glTexCoordPointer(2, GL_FLOAT, 0, &simpleTexCoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    _renderedVerts += 4;
    glPopMatrix();

    ++_renderedObjects;
}

void Renderer::renderTileArray(Tile * const *tiles, unsigned int size, const Vector& start, const Vector& step, const Vector& halfsize)
{
    Vector trans = start;
    {
        int skipped = 0;
        // skip forward until first usable tile is in tiles[0], or abort if nothing found
        while(size && !*tiles)
        {
            --size;
            ++tiles;
            ++skipped;
        }
        if(!size)
            return;

        trans += step * skipped;
    }

    glPushMatrix();

    _enableVertexAndTexCoords();

    const float w2 = halfsize.x;
    const float h2 = halfsize.y;

    do
    {
        Tile *tile = *tiles++;
        if(tile)
        {
            tile->getTexture()->apply();

            {
                const GLfloat vertexData[] =
                {
                    -w2, -h2, // upper left
                    +w2, -h2, // upper right
                    -w2, +h2, // lower left
                    +w2, +h2, // lower right
                };

                glVertexPointer(2, GL_FLOAT, 0, &vertexData);
            }

            {
                const UV& ultc = tile->getULTC();
                const UV& lrtc = tile->getLRTC();
                const GLfloat texCoords[] =
                {
                    ultc.u, ultc.v, // upper left
                    lrtc.u, ultc.v, // upper right
                    ultc.u, lrtc.v, // lower left
                    lrtc.u, lrtc.v, // lower right
                };
                glTexCoordPointer(2, GL_FLOAT, 0, &texCoords);
            }

            glTranslatef(trans.x, trans.y, trans.z);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            _renderedVerts += 4;
            trans.x = trans.y = trans.z = 0;

            ++_renderedObjects;
        }
        trans += step;
    }
    while(--size);

    glPopMatrix();
}

void Renderer::render2DVertexArray(float *verts, unsigned int size)
{
    glVertexPointer(2, GL_FLOAT, 0, verts);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, size);
    _renderedVerts += 4;
}

void Renderer::_enableVertexAndTexCoords()
{
    if(_clientState != CLS_VERT_TEXCOORD)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        _clientState = CLS_VERT_TEXCOORD;
    }
}

void Renderer::_disableVertexAndTexCoords()
{
    if(_clientState != CLS_VERT_TEXCOORD)
    {
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        _clientState = CLS_NONE;
    }
}

Texture *Renderer::createNullTexture()
{
    GLTexture *tex = new GLTexture("");
    tex->setID(0);
    return tex;
}

void Renderer::drawLine(float x1, float y1, float x2, float y2, float width, float r, float g, float b, float a)
{
    // FIXME: this isn't so nice
    glBindTexture(GL_TEXTURE_2D, 0);
    Texture::clearLastApplied();

    glLineWidth(width);
    _applyBlendType(BLEND_DEFAULT);

    _pushColor();
    _multColor(r, g, b, a);
    _applyColor();
    const GLfloat verts[] =
    {
        x1, y1,
        x2, y2,
    };
    glVertexPointer(2, GL_FLOAT, 0, &verts);
    glDrawArrays(GL_LINES, 0, 2);
    _popColor();
}

void Renderer::drawAABB(float x1, float y1, float x2, float y2, float rotation, float r, float g, float b, float a)
{
    // FIXME: this isn't so nice
    glBindTexture(GL_TEXTURE_2D, 0);
    Texture::clearLastApplied();

    _applyBlendType(BLEND_DEFAULT);

    _pushColor();
    _multColor(r, g, b, a);
    _applyColor();
    const GLfloat verts[] =
    {
        x1, y1,
        x2, y1,
        x1, y2,
        x2, y2,
    };
    glPushMatrix();
    glRotatef(-rotation, 0, 0, 1); 
    glVertexPointer(2, GL_FLOAT, 0, &verts);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glPopMatrix();
    _popColor();
}


void Renderer::drawCircle(float x, float y, float radius, float r, float g, float b, float a)
{
    // FIXME: this isn't so nice
    glBindTexture(GL_TEXTURE_2D, 0);
    Texture::clearLastApplied();

    _pushColor();
    _multColor(r, g, b, a);
    _applyColor();
    _applyBlendType(BLEND_DEFAULT);

    GLfloat verts[2 * (360 / 5)];

    for(int i = 0; i < (360/5); ++i)
    {
        const float rad = degToRad(i * 5); // FIXME: just use float step
        verts[2 * i    ] = cosf(rad) * radius;
        verts[2 * i + 1] = sinf(rad) * radius;
    }

    glVertexPointer(2, GL_FLOAT, 0, verts);
    glDrawArrays(GL_POLYGON, 0, (360 / 5));
    _renderedVerts += (360 / 5);
    _popColor();
}

void Renderer::_multColor(float r, float g, float b, float a)
{
    _RenderColor& col = _topColor();
    col.r *= r;
    col.g *= g;
    col.b *= b;
    col.a *= a;
}

void Renderer::_popColor()
{
    // at least one element must stay in the vector
    assert(_colorstack.size() > 1);
    _colorstack.pop_back();
}

void Renderer::_pushColor()
{
    _colorstack.push_back(_topColor());
}

void Renderer::_applyColor()
{
    const _RenderColor& col = _topColor();
    glColor4f(col.r, col.g, col.b, col.a);
}

