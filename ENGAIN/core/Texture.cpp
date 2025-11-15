#include "Texture.h"
#include "Logger.h"
#include <SDL2/SDL_image.h>

namespace ENGAIN {

Texture::Texture() : texture(nullptr), width(0), height(0) {}

Texture::~Texture() {
    free();
}

bool Texture::loadFromFile(const std::string& path, SDL_Renderer* renderer) {
    free();
    
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (!loadedSurface) {
        Logger::getInstance().error("Unable to load image " + path + "! SDL_image Error: " + IMG_GetError());
        return false;
    }
    
    texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (!texture) {
        Logger::getInstance().error("Unable to create texture from " + path + "! SDL Error: " + SDL_GetError());
    } else {
        width = loadedSurface->w;
        height = loadedSurface->h;
        Logger::getInstance().info("Loaded texture: " + path);
    }
    
    SDL_FreeSurface(loadedSurface);
    
    return texture != nullptr;
}

void Texture::free() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
        width = 0;
        height = 0;
    }
}

void Texture::render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip) {
    SDL_Rect renderQuad = {x, y, width, height};
    
    if (clip != nullptr) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    
    SDL_RenderCopy(renderer, texture, clip, &renderQuad);
}

void Texture::renderEx(SDL_Renderer* renderer, int x, int y, double angle, 
                       SDL_Point* center, SDL_RendererFlip flip) {
    SDL_Rect renderQuad = {x, y, width, height};
    SDL_RenderCopyEx(renderer, texture, nullptr, &renderQuad, angle, center, flip);
}

void Texture::renderScaled(SDL_Renderer* renderer, int x, int y, int w, int h, 
                           SDL_RendererFlip flip) {
    SDL_Rect renderQuad = {x, y, w, h};
    SDL_RenderCopyEx(renderer, texture, nullptr, &renderQuad, 0, nullptr, flip);
}

void Texture::setColor(uint8_t r, uint8_t g, uint8_t b) {
    SDL_SetTextureColorMod(texture, r, g, b);
}

void Texture::setBlendMode(SDL_BlendMode blending) {
    SDL_SetTextureBlendMode(texture, blending);
}

void Texture::setAlpha(uint8_t alpha) {
    SDL_SetTextureAlphaMod(texture, alpha);
}

} // namespace ENGAIN
