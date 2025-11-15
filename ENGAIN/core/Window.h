#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <memory>

namespace ENGAIN {

struct Color {
    uint8_t r, g, b, a;
    
    Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}
};

class Window {
public:
    Window(const std::string& title = "ENGAIN Window",
           int width = 800,
           int height = 600,
           bool resizable = true);
    
    ~Window();
    
    bool initialize();
    void handleEvents();
    void clear(const Color& color = Color(0, 0, 0));
    void present();
    void close();
    
    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool isRunning() const { return running; }
    bool isFocused() const { return focused; }
    SDL_Renderer* getRenderer() const { return renderer; }
    SDL_Window* getSDLWindow() const { return window; }
    
    // Setters
    void setTitle(const std::string& title);
    
private:
    std::string title;
    int width;
    int height;
    bool resizable;
    
    bool running;
    bool focused;
    bool initialized;
    
    SDL_Window* window;
    SDL_Renderer* renderer;
};

} // namespace ENGAIN
