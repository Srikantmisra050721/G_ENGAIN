#include "../ENGAIN/core/Window.h"
#include "../ENGAIN/core/Texture.h"
#include <SDL2/SDL.h>

using namespace ENGAIN;

int main(int argc, char* argv[]) {
    // Create window
    Window window("ENGAIN - Kitty Sprite", 1200, 800, true);
    if (!window.initialize()) {
        return -1;
    }
    
    // Load kitty sprite
    Texture kittySprite;
    if (!kittySprite.loadFromFile("assets/kitty.png", window.getRenderer())) {
        return -1;
    }
    
    Color bgColor(20, 20, 30);
    
    // Main loop
    while (window.isRunning()) {
        // Handle events
        window.handleEvents();
        
        // Clear screen
        window.clear(bgColor);
        
        SDL_Renderer* renderer = window.getRenderer();
        
        // Calculate sprite parameters
        int centerX = window.getWidth() / 2;
        int centerY = window.getHeight() / 2;
        int spriteSize = 200;
        
        // Draw kitty sprite
        SDL_Rect dstRect = {
            centerX - spriteSize / 2,
            centerY - spriteSize / 2,
            spriteSize,
            spriteSize
        };
        
        SDL_RenderCopyEx(renderer, kittySprite.getSDLTexture(), nullptr, &dstRect, 
                        0, nullptr, SDL_FLIP_NONE);
        
        // Present
        window.present();
    }
    
    return 0;
}
