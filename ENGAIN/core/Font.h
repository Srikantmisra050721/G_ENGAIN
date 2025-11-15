#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <unordered_map>
#include "Window.h"

namespace ENGAIN {

class Font {
public:
    Font();
    ~Font();
    
    bool loadFromFile(const std::string& path, int size);
    void free();
    
    SDL_Texture* renderText(SDL_Renderer* renderer, const std::string& text, SDL_Color color);
    void drawText(SDL_Renderer* renderer, const std::string& text, int x, int y, SDL_Color color);
    
    int getFontHeight() const;
    void getTextSize(const std::string& text, int* w, int* h);
    
private:
    TTF_Font* font;
    int fontSize;
};

class TextRenderer {
public:
    static TextRenderer& getInstance();
    
    bool initialize();
    void shutdown();
    
    bool loadFont(const std::string& name, const std::string& path, int size);
    void drawText(SDL_Renderer* renderer, const std::string& text, int x, int y, 
                  const std::string& fontName = "default", SDL_Color color = {255, 255, 255, 255});
    
private:
    TextRenderer();
    ~TextRenderer();
    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;
    
    bool initialized;
    std::unordered_map<std::string, Font*> fonts;
};

} // namespace ENGAIN
