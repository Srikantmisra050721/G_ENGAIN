#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <memory>

namespace ENGAIN {

class Texture {
public:
    Texture();
    ~Texture();
    
    bool loadFromFile(const std::string& path, SDL_Renderer* renderer);
    void free();
    
    void render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = nullptr);
    void renderEx(SDL_Renderer* renderer, int x, int y, double angle = 0.0, 
                  SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);
    void renderScaled(SDL_Renderer* renderer, int x, int y, int w, int h, 
                     SDL_RendererFlip flip = SDL_FLIP_NONE);
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    void setColor(uint8_t r, uint8_t g, uint8_t b);
    void setBlendMode(SDL_BlendMode blending);
    void setAlpha(uint8_t alpha);
    
    SDL_Texture* getSDLTexture() const { return texture; }

private:
    SDL_Texture* texture;
    int width;
    int height;
};

} // namespace ENGAIN
