#include "Input.h"

namespace ENGAIN {

Input::Input() {}

Input& Input::getInstance() {
    static Input instance;
    return instance;
}

void Input::update() {
    previousKeys = currentKeys;
    
    const Uint8* keyState = SDL_GetKeyboardState(nullptr);
    
    // Update common keys
    SDL_Keycode keys[] = {
        SDLK_w, SDLK_a, SDLK_s, SDLK_d,
        SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT,
        SDLK_SPACE, SDLK_ESCAPE, SDLK_RETURN,
        SDLK_LSHIFT, SDLK_RSHIFT
    };
    
    for (SDL_Keycode key : keys) {
        SDL_Scancode scancode = SDL_GetScancodeFromKey(key);
        currentKeys[key] = keyState[scancode];
    }
}

bool Input::isKeyDown(SDL_Keycode key) const {
    auto it = currentKeys.find(key);
    return it != currentKeys.end() && it->second;
}

bool Input::isKeyPressed(SDL_Keycode key) const {
    auto curr = currentKeys.find(key);
    auto prev = previousKeys.find(key);
    
    bool currentlyDown = (curr != currentKeys.end() && curr->second);
    bool previouslyDown = (prev != previousKeys.end() && prev->second);
    
    return currentlyDown && !previouslyDown;
}

bool Input::isKeyReleased(SDL_Keycode key) const {
    auto curr = currentKeys.find(key);
    auto prev = previousKeys.find(key);
    
    bool currentlyDown = (curr != currentKeys.end() && curr->second);
    bool previouslyDown = (prev != previousKeys.end() && prev->second);
    
    return !currentlyDown && previouslyDown;
}

} // namespace ENGAIN
