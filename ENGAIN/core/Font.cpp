#include "Font.h"
#include "Logger.h"

namespace ENGAIN {

// Font implementation
Font::Font() : font(nullptr), fontSize(16) {}

Font::~Font() {
    free();
}

bool Font::loadFromFile(const std::string& path, int size) {
    free();
    
    font = TTF_OpenFont(path.c_str(), size);
    if (!font) {
        Logger::getInstance().error("Failed to load font " + path + "! SDL_ttf Error: " + TTF_GetError());
        return false;
    }
    
    fontSize = size;
    Logger::getInstance().info("Loaded font: " + path + " (size: " + std::to_string(size) + ")");
    return true;
}

void Font::free() {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}

SDL_Texture* Font::renderText(SDL_Renderer* renderer, const std::string& text, SDL_Color color) {
    if (!font) return nullptr;
    
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) {
        Logger::getInstance().error("Failed to render text! SDL_ttf Error: " + std::string(TTF_GetError()));
        return nullptr;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

void Font::drawText(SDL_Renderer* renderer, const std::string& text, int x, int y, SDL_Color color) {
    SDL_Texture* texture = renderText(renderer, text, color);
    if (!texture) return;
    
    int w, h;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    
    SDL_Rect dstRect = {x, y, w, h};
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
    
    SDL_DestroyTexture(texture);
}

int Font::getFontHeight() const {
    if (!font) return 0;
    return TTF_FontHeight(font);
}

void Font::getTextSize(const std::string& text, int* w, int* h) {
    if (!font) {
        if (w) *w = 0;
        if (h) *h = 0;
        return;
    }
    TTF_SizeText(font, text.c_str(), w, h);
}

// TextRenderer implementation
TextRenderer::TextRenderer() : initialized(false) {}

TextRenderer::~TextRenderer() {
    shutdown();
}

TextRenderer& TextRenderer::getInstance() {
    static TextRenderer instance;
    return instance;
}

bool TextRenderer::initialize() {
    if (initialized) return true;
    
    if (TTF_Init() == -1) {
        Logger::getInstance().error("SDL_ttf could not initialize! SDL_ttf Error: " + std::string(TTF_GetError()));
        return false;
    }
    
    Logger::getInstance().info("SDL_ttf initialized");
    initialized = true;
    return true;
}

void TextRenderer::shutdown() {
    for (auto& pair : fonts) {
        delete pair.second;
    }
    fonts.clear();
    
    if (initialized) {
        TTF_Quit();
        initialized = false;
        Logger::getInstance().info("SDL_ttf shutdown");
    }
}

bool TextRenderer::loadFont(const std::string& name, const std::string& path, int size) {
    if (!initialized) {
        Logger::getInstance().error("TextRenderer not initialized!");
        return false;
    }
    
    Font* font = new Font();
    if (!font->loadFromFile(path, size)) {
        delete font;
        return false;
    }
    
    fonts[name] = font;
    return true;
}

void TextRenderer::drawText(SDL_Renderer* renderer, const std::string& text, int x, int y,
                           const std::string& fontName, SDL_Color color) {
    auto it = fonts.find(fontName);
    if (it == fonts.end()) {
        Logger::getInstance().warning("Font not found: " + fontName);
        return;
    }
    
    it->second->drawText(renderer, text, x, y, color);
}

} // namespace ENGAIN
