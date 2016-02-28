#include "MouseState.h"
#include "SDLMaster.h"

bool MouseState::isLeft() const { return buttons & SDL_BUTTON_LMASK; }
bool MouseState::isRight() const { return buttons & SDL_BUTTON_RMASK; }
bool MouseState::isMiddle() const { return buttons & SDL_BUTTON_MMASK; }
