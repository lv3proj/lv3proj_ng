#include <SDL/SDL.h>

#include "Renderer.h"
#include "OpenGLAPI.h"
#include "GL/gl.h"
#include "Engine.h"
#include "SDLSurfaceResource.h"
#include "GLTexture.h"
#include "Quad.h"


Renderer::Renderer()
: screen(NULL)
{
    memset(&settings, 0, sizeof(settings));
    memset(&active, 0, sizeof(settings));
    globalResolutionScale = Vector(1, 1, 1);
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

    char drv[256];
    SDL_VideoDriverName(&drv[0], 256);
    log("Video driver: %s", drv);


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
        engine->virtualOffX = ((x - settings.virtualW)/2);
    else
        engine->virtualOffX = 0;


    diff = y - settings.virtualH;
    if (diff > 0)
        engine->virtualOffY = ((y - settings.virtualH)/2);
    else
        engine->virtualOffY = 0;
}

void Renderer::setupRenderPositionAndScale()
{
    glLoadIdentity();
    // FIXME: not sure where to store the camera, this seems to make sense

    const Vector& globalScale = engine->camera.scale;
    const Vector& cameraPos = engine->camera.position;
    const Vector& cameraOffset = engine->camera.offset;
    //const Vector& cameraRot = engine->camera->rotation;

    //glRotatef(cameraRot.z, 0, 0, 1);
    glScalef(globalScale.x * globalResolutionScale.x,
             globalScale.y * globalResolutionScale.y,
             globalScale.z * globalResolutionScale.z);

    glTranslatef(-(cameraPos.x+cameraOffset.x), -(cameraPos.y+cameraOffset.y), -(cameraPos.z+cameraOffset.z));
    //glRotatef(180, 0, 1, 0);
}



void Renderer::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
}

void Renderer::show()
{
    SDL_GL_SwapBuffers();
}


const bool DEBUG_RENDER = true;

void Renderer::renderObject(const RenderObject *ro)
{
    const Vector renderPos = ro->position + ro->offset;
    const Vector renderRot = ro->rotation + ro->rotation2;
    const float renderAlpha = ro->alpha.x + ro->alpha2.x;
    const Vector renderCol = ro->color;

    glPushMatrix();
    glTranslatef(renderPos.x, renderPos.y, renderPos.z);

    glRotatef(renderRot.x, 0, 0, 1); 
    /*if (isfh())
    {
    glDisable(GL_CULL_FACE);
    glRotatef(180, 0, 1, 0);
    }*/

    glScalef(ro->scale.x, ro->scale.y, 1);

    glColor4f(renderCol.x, renderCol.y, renderCol.z, renderAlpha);

    _applyBlendType(ro->getBlendType());

    ro->onRender();

    // TODO: render children


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
}

void Renderer::_applyBlendType(BlendType blend)
{
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
}

void Renderer::renderQuad(const Quad *q)
{
    Texture *tex = q->getTexture();
    if(!tex)
        return;

    tex->apply();
    const float w2 = q->width / 2.0f;
    const float h2 = q->height / 2.0f;
    const Vector& upperLeftTextureCoordinates = q->upperLeftTextureCoordinates;
    const Vector &lowerRightTextureCoordinates = q->lowerRightTextureCoordinates;

    glBegin(GL_QUADS);
    {
        glTexCoord2f(upperLeftTextureCoordinates.x, 1.0f-upperLeftTextureCoordinates.y);
        glVertex2f(-w2, +h2);

        glTexCoord2f(lowerRightTextureCoordinates.x, 1.0f-upperLeftTextureCoordinates.y);
        glVertex2f(+w2, +h2);

        glTexCoord2f(lowerRightTextureCoordinates.x, 1.0f-lowerRightTextureCoordinates.y);
        glVertex2f(+w2, -h2);

        glTexCoord2f(upperLeftTextureCoordinates.x, 1.0f-lowerRightTextureCoordinates.y);
        glVertex2f(-w2, -h2);
    }
    glEnd();
}