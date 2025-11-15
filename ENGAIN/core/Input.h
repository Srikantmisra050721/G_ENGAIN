#pragma once

#include <SDL2/SDL.h>
#include <unordered_map>

namespace ENGAIN {

class Input {
public:
    static Input& getInstance();
    
    void update();
    
    bool isKeyDown(SDL_Keycode key) const;
    bool isKeyPressed(SDL_Keycode key) const;
    bool isKeyReleased(SDL_Keycode key) const;
    
private:
    Input();
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;
    
    std::unordered_map<SDL_Keycode, bool> currentKeys;
    std::unordered_map<SDL_Keycode, bool> previousKeys;
};

} // namespace ENGAIN
