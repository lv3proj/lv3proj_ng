#include "AnimatedTexture.h"
#include <assert.h>
#include "log.h"

AnimatedTexture::AnimatedTexture(Anim *a)
 : Texture(a->name()), _ani(a)
{
    a->incref();
    _InitAnim();
}

AnimatedTexture::~AnimatedTexture()
{
    _ani->decref();
}

void AnimatedTexture::doApply()
{
    if(_activeTex)
        _activeTex->apply();
}

void AnimatedTexture::update(float dt)
{
    if(_timer.update(dt))
        _NextFrame();
}

bool AnimatedTexture::reload()
{
    return true; // nothing to do - the other textures will be reloaded on their own.
}

void AnimatedTexture::_InitAnim()
{
    assert(_ani->first); // the parser must set this
    _curFrameStore = _ani->first;
    SetFrameID(0);
}

void AnimatedTexture::_NextFrame()
{
    ++_curFrameIdx;

    if(_curFrameIdx < _curFrameStore->store.size())
    {
        SetFrameID(_curFrameIdx);
        return;
    }

    switch(_curFrameStore->cmd.action)
    {
    case ANIMCMD_REPEAT:
        SetFrameID(0);
        break;

    case ANIMCMD_JUMP:
        SetFrameName(_curFrameStore->cmd.param.c_str());
        break;

    default:
        assert(false);
    }
}

void AnimatedTexture::SetFrameID(unsigned int frame)
{
    assert(frame < _curFrameStore->store.size());
    _curFrame = &(_curFrameStore->store[frame]);
    _timer.set(_curFrame->frametime / 1000.0f); // FIXME: this should also be able to handle dropped frames
    _curFrameIdx = frame;

    _activeTex = _curFrame->GetTexture();
    if(_activeTex)
    {
        width = _activeTex->getWidth();
        height = _activeTex->getHeight();
    }
}

void AnimatedTexture::SetFrameName(const char *name)
{
    // only change frame if the name really changes
    if(_curFrameStore->name == name)
        return;

    AnimMap::iterator am = _ani->anims.find(name);
    if(am != _ani->anims.end())
    {
        _curFrameStore = &(am->second);

        // list of frames is empty, but there may be a jump command to another frame
        if(_curFrameStore->store.empty())
        {
            if(_curFrameStore->cmd.action == ANIMCMD_JUMP)
                SetFrameName(_curFrameStore->cmd.param.c_str());
            else
                logerror("AnimatedTexture '%s' framename '%s' does not contain any frames!", this->name(), name); 
            return;
        }

        SetFrameID(0);
    }
    else
    {
        logerror("AnimatedTexture '%s' SetFrameName(%s) - frame not found", this->name(), name);
    }
}

